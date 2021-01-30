#include "RenderingEngine.hpp"

namespace rendering
{
    void RenderingEngine::setUniforms(shading::Shader& shader)
    {
        shader.setUniformFloat("time", (float)glfwGetTime());
        shader.setUniformInt("pick", pickingResult);

        if (shader.getRenderPass() == shading::RenderPass::LIGHTING)
            setLightingTextureUniforms(shader);
        else if (shader.getRenderPass() == shading::RenderPass::FORWARD) {
            for (unsigned int i = 0; i < MAX_SHADOW_MAPS; ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
                shader.setUniformInt("shadowMap[" + std::to_string(i) + "]", i);
            }
        }
    }

    void RenderingEngine::setLightingTextureUniforms(shading::Shader& shader)
    {
        GLuint target = MSAA_SAMPLES == 0 ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(target, gPosition);
        shader.setUniformInt("gPosition", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(target, gNormal);
        shader.setUniformInt("gNormal", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(target, gAmbient);
        shader.setUniformInt("gAmbient", 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(target, gDiffuse);
        shader.setUniformInt("gDiffuse", 3);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(target, gSpecular);
        shader.setUniformInt("gSpecular", 4);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ssaoBlur ? ssaoBlurColor : ssaoColor);
        shader.setUniformInt("ssao", 5);
        shader.setUniformInt("enableAO", useSSAO);
        for (unsigned int i = 0; i < MAX_SHADOW_MAPS; ++i) {
            glActiveTexture(GL_TEXTURE6 + i);
            glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
            shader.setUniformInt("shadowMap[" + std::to_string(i) + "]", 6 + i);
        }
    }

    void RenderingEngine::renderScreen(rendering::components::Camera& camera)
    {
        shading::Shader& shader = *screenShader;
        rendering::systems::activateShader(registry, camera, shader);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void RenderingEngine::renderQuad(GLuint image)
    {
        shading::Shader& shader = *quadShader;
        shader.use();
        //camera.applyViewProjection(shader);
        //rendering::systems::activateShader(registry, camera, shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, image);
        shader.setUniformInt("image", 0);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void RenderingEngine::renderSSAO(rendering::components::Camera& camera)
    {
        shading::Shader& shader = *ssaoShader;
        shader.use();
        camera.applyViewProjection(shader);

        float width = (float) getFramebufferWidth();
        float height = (float) getFramebufferHeight();

        glm::mat4& proj = camera.getProjectionMatrix();
        // calculate the height in pixels of a 1m object at z=-1m
        glm::vec4 p1 = proj * glm::vec4(0, -0.5, -1, 1);
        glm::vec4 p2 = proj * glm::vec4(0, 0.5, -1, 1);
        float scale = height * abs(p1.y / p1.w - p2.y / p2.w) / 2.f;
        shader.setUniformFloat("screenScale", scale);
       
        // parameters for inverse projection calculations (Eqn. 3 in McGuire paper)
        glm::vec4 invProjection(
            (1.f - proj[0][2]) / proj[0][0],
            2.f / (width * proj[0][0]),
            (1.f + proj[1][2]) / proj[1][1],
            2.f / (height * proj[1][1])
        );
        shader.setUniformVec4("invProjection", invProjection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, MSAA_SAMPLES == 0 ? gZ : ssaoZ);
        shader.setUniformInt("gZ", 0);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void RenderingEngine::renderSSAOZ(rendering::components::Camera& camera)
    {
        shading::Shader& shader = *ssaoZShader;
        shader.use();
        //camera.applyViewProjection(shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gZ);
        shader.setUniformInt("depth", 0);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void RenderingEngine::renderSSAOBlur(GLuint input, bool first)
    {
        glm::vec2 axis = first ? glm::vec2(1, 0) : glm::vec2(0, 1);
        shading::Shader& shader = *blurShader;
        if (first) shader.use();

        shader.setUniformVec2("axis", axis);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input);
        shader.setUniformInt("image", 0);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void RenderingEngine::renderFXAA()
    {
        shading::Shader& shader = *fxaaShader;
        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mainColor);
        shader.setUniformInt("image", 0);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void RenderingEngine::doPicking()
    {
        // read picking framebuffer
        index = (index + 1) % 2;
        nextIndex = (index + 1) % 2;
        glNamedFramebufferReadBuffer(pickingFramebuffer, GL_FRONT);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[index]);
        double posX, posY;
        glfwGetCursorPos(window, &posX, &posY);
        if (posX <= getFramebufferWidth() && posY <= getFramebufferHeight() && posX >= 0 && posY >= 0) {
            glReadPixels((int)posX, getFramebufferHeight() - (int)posY, 1, 1, GL_BGRA, GL_UNSIGNED_BYTE, 0);

            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[nextIndex]);
            GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
            if (ptr) {
                GLubyte blue = ptr[0];
                GLubyte green = ptr[1];
                GLubyte red = ptr[2];
                pickingResult = ((uint32_t)blue) + (((uint32_t)green) << 8) + (((uint32_t)red) << 16);
                glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            }
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        }
        else {
            pickingResult = 0xffffff;
        }
    }

    void RenderingEngine::render()
    {
        // prepare gui for render
        gui::startFrame();

        // select default shader for rendering system
        auto defaultShader = showWireframe ? wireframeShader : mainShader;

        auto camera = updateCamera(mainCamera, (float)width / (float)height);
        //auto shadow = updateCamera(shadowCamera, 1.f);
        std::vector<rendering::components::Camera> shadows;
        for (auto& cam : shadowCameras)
            shadows.push_back(updateCamera(cam, 1.f));

        systems::renderUpdateTransforms(registry, camera, shadowCameras, defaultShader, showWireframe);

        for (unsigned int i = 0; i < std::min(MAX_SHADOW_MAPS, (int) shadowCameras.size()); ++i) {
            systems::renderUpdateMVPs(registry, shadows[i], [this, i](auto* mesh) {
                auto& castShadow = registry.ctx<systems::ShadowMapping>().castShadow;
                auto entry = castShadow.find(mesh);
                return entry == castShadow.end() || i > entry->second;
            });

            //// SHADOW PASS
            glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffers[i]);
            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, SHADOW_MAP_RES, SHADOW_MAP_RES);

            systems::renderShadowMap(registry, shadows[i], shadowZShader);
        }

        // recalculate MVPs for main camera
        systems::renderUpdateMVPs(registry, camera, [](const auto* mesh) { return false; }); // exclude nothing

        // PICKING PASS
        glBindFramebuffer(GL_FRAMEBUFFER, pickingFramebuffer);
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, getFramebufferWidth(), getFramebufferHeight());

        systems::renderPicking(registry, camera, pickingShader);
        doPicking();

        // DEFERRED - GEOMETRY PASS
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        systems::renderDeferredGPass(registry, camera);

        // SSAO
        if (useSSAO) {
            if (MSAA_SAMPLES > 0) {
                // this pass resolves the multisampling of the texture with high precision camera space z values
                glBindFramebuffer(GL_FRAMEBUFFER, ssaoZBuffer);
                glClearColor(0.f, 0.f, 0.f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                renderSSAOZ(camera);
            }
            GLuint zTexture = MSAA_SAMPLES == 0 ? gZ : ssaoZ;
            // this is important! generating mipmaps of the z texture reduces cache misses for AO samples
            glBindTexture(GL_TEXTURE_2D, zTexture);
            glGenerateMipmap(GL_TEXTURE_2D);

            // noisy AO sample pass
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer);
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderSSAO(camera);

            // AO blur passes (one horizontal, one vertical)
            if (ssaoBlur)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBufferI);
                glClearColor(0.f, 0.f, 0.f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                renderSSAOBlur(ssaoColor, true);

                glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);
                glClearColor(0.f, 0.f, 0.f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                renderSSAOBlur(ssaoBlurColorI, false);
            }
        }


        // DEFERRED - LIGHTING PASS
        glBindFramebuffer(GL_FRAMEBUFFER, mainBuffer);
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if (true) {
            renderScreen(camera);

            //copy depth buffer of geometry pass to main framebuffer
            glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mainBuffer);
            glBlitFramebuffer(
                0, 0, getFramebufferWidth(), getFramebufferHeight(),
                0, 0, getFramebufferWidth(), getFramebufferHeight(),
                GL_DEPTH_BUFFER_BIT, GL_NEAREST
            );
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);    // additive blending
            glDepthMask(GL_FALSE);          // disable depth writing (lights should overlap)
            glDisable(GL_DEPTH_TEST);
            glCullFace(GL_FRONT);

            rendering::systems::renderDeferredLightingPass(registry, camera);

            glCullFace(GL_BACK);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            rendering::systems::renderForward(registry, camera);
            glDisable(GL_BLEND);
        }
        else {
            //renderQuad(ssaoBlur ? ssaoBlurColor : ssaoColor);
            renderQuad(pickingColorbuffer);

            glDisable(GL_BLEND);
        }
        
        game->render(this);

        if (MSAA_SAMPLES == 0 && useFXAA && !showWireframe) {
            // use fxaa
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderFXAA();
        }
        else {
            // no fxaa, blit mainBuffer to default framebuffer
            glBindFramebuffer(GL_READ_FRAMEBUFFER, mainBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(
                0, 0, getFramebufferWidth(), getFramebufferHeight(),
                0, 0, getFramebufferWidth(), getFramebufferHeight(),
                GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, GL_NEAREST
            );
        }


        renderDebugWindow();
        gui::render();

        glfwSwapBuffers(window);
    }
}