#include "RenderingEngine.hpp"

namespace rendering
{
    void RenderingEngine::setUniforms(shading::Shader& shader)
    {
        shader.setUniformFloat("time", (float)glfwGetTime());
        shader.setUniformInt("pick", pickingResult);

        if (shader.getRenderPass() == shading::RenderPass::LIGHTING)
            setGeometryTextureUniforms(shader);
    }

    void RenderingEngine::setGeometryTextureUniforms(shading::Shader& shader)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gPosition);
        shader.setUniformInt("gPosition", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gNormal);
        shader.setUniformInt("gNormal", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gAmbient);
        shader.setUniformInt("gAmbient", 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gDiffuse);
        shader.setUniformInt("gDiffuse", 3);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gSpecular);
        shader.setUniformInt("gSpecular", 4);
    }

    void RenderingEngine::renderQuad(rendering::components::Camera& camera)
    {
        //quadShader->use();
        rendering::systems::activateShader(registry, camera, *quadShader);
        setGeometryTextureUniforms(*quadShader);

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
        rendering::systems::renderUpdateTransforms(registry, camera, defaultShader, showWireframe);

        // PICKING PASS
        glBindFramebuffer(GL_FRAMEBUFFER, pickingFramebuffer);
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        rendering::systems::renderPicking(registry, camera, pickingShader);
        doPicking();

        // DEFERRED - GEOMETRY PASS
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        rendering::systems::renderDeferredGPass(registry, camera);

        // DEFERRED - LIGHTING PASS
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderQuad(camera);

        //copy depth buffer of geometry pass to the default framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, getFramebufferWidth(), getFramebufferHeight(),
            0, 0, getFramebufferWidth(), getFramebufferHeight(),
            GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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


        game->render(this);
        renderDebugWindow();
        gui::render();

        glfwSwapBuffers(window);
    }
}