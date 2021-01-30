#version 400 core
// based on https://catlikecoding.com/unity/tutorials/advanced-rendering/fxaa/

// 0.0625 - skip more, faster
// 0.0312 - skip less, slower
#define CONTRAST 0.02

// 0.25 - low quality
// 0.125 - high quality
#define RELATIVE 0.225

#define BLEND 0.25

#define EDGE 0.25
#define EDGE_STEPS 16

in vec2 uv;
uniform sampler2D image;
out vec3 color;

vec2 size;
vec2 texelSize;

vec3 fetchPixel(vec2 coord, vec2 pxOffset) {
    //return texelFetch(image, coord, 0).xyz;
    return texture(image, coord + pxOffset * texelSize).xyz;
}

// luminance values
float m;
float n, s, e, w;
float ne = 0, nw = 0, se = 0, sw = 0;   // might not be used
// contrast
float contrast;

// source: CG lecture slides
vec3 luminanceFactors = vec3(0.3, 0.59, 0.11);

float luminance(vec3 rgb) {
    return dot(rgb, luminanceFactors);
}

float luminance(vec2 coord, vec2 pxOffset) {
    return luminance(fetchPixel(coord, pxOffset));
}

float localContrast(out float highest) {
    float maxVal = max(n, max(max(s, w), max(e, m)));
    float minVal = min(n, min(min(s, w), min(e, m)));
    highest = maxVal;
    return maxVal - minVal;
}

bool skip(float highest) {
    float threshold = max(CONTRAST, highest * RELATIVE);
    return contrast < threshold;
}

float pixelBlend() {
    // weighted average of neighbor luminance
    float blend = 2.0 * (n + s + e + w);
    blend += ne + nw + se + sw;
    blend /= 12.0;
    // contrast by difference from center
    blend = abs(blend - m);
    // normalize to contrast and smooth result
    blend = clamp(blend / contrast, 0, 1);
    return pow(smoothstep(0, 1, blend), 2) * BLEND;
}

bool detectEdge() {
    float horizontal = 2.0 * abs(n + s - 2*m)
        + abs(ne + se - 2*e) + abs(nw + sw - 2*w);
    float vertical = 2.0 * abs(e + w - 2*m)
        + abs(nw + ne - 2*n) + abs(sw + se - 2*s);
    return horizontal >= vertical;
}

vec2 gradients(bool horizontal, out float opposite, out float gradient) {
    float pos, neg;
    vec2 pixelStep;
    if (horizontal) {
        pos = n; neg = s;
        pixelStep = vec2(0, 1);
    } else {
        pos = e; neg = w;
        pixelStep = vec2(1, 0);
    }
    float pGradient = abs(pos - m);
    float nGradient = abs(neg - m);

    if (pGradient < nGradient) {
        pixelStep = -pixelStep;
        opposite = neg;
        gradient = nGradient;
    } else {
        opposite = pos;
        gradient = pGradient;
    }

    return pixelStep;
}

float walkEdge(vec2 start, bool horizontal, float direction, float edgeLum, float threshold, out float delta) {
    vec2 edgeStep = horizontal ? vec2(direction, 0) : vec2(0, direction);
    vec2 edgeOffset = start + edgeStep;

    delta = luminance(uv, edgeOffset) - edgeLum;
    bool pAtEnd = abs(delta) >= threshold;
    
    for (int i = 0; i < EDGE_STEPS; i++) {
        if (pAtEnd) break;

        edgeOffset += edgeStep;
        delta = luminance(uv, edgeOffset) - edgeLum;
        pAtEnd = abs(delta) >= threshold;
    }

    return abs(horizontal ? edgeOffset.x : edgeOffset.y);
}

float edgeBlend(out vec2 pixelStep) {
    // find edge orientation
    bool horizontal = detectEdge();
    // calculate edge luminance gradient
    float opposite, gradient;
    pixelStep = gradients(horizontal, opposite, gradient);


    float edge = (m + opposite) * 0.5;
    float threshold = gradient * BLEND;

    // edge sample point uv coordinates
    vec2 edgeOffset = pixelStep * 0.5;
    float pDelta, nDelta;
    float pDist = walkEdge(edgeOffset, horizontal, 1.0, edge, threshold, pDelta);
    float nDist = walkEdge(edgeOffset, horizontal, -1.0, edge, threshold, nDelta);

    // return 0 edge blend if gradient points in the wrong direction
    float minDist;
    bool deltaSign;
    if (pDist <= nDist) {
        minDist = pDist;
        deltaSign = pDelta >= 0;
    } else {
        minDist = nDist;
        deltaSign = nDelta >= 0;
    }
    if (deltaSign == (m - edge >= 0)) return 0.0;
    
    // blend more towards the end point
    return 0.5 - minDist / (pDist + nDist);
}

void main() {
	size = textureSize(image, 0);
    texelSize = 1.0 / vec2(size);

    // read center and neighbor pixel values from texture
    //ivec2 screenCoord = ivec2(gl_FragCoord);
    n = luminance(uv, vec2( 0,  1));
    s = luminance(uv, vec2( 0, -1));
    e = luminance(uv, vec2( 1,  0));
    w = luminance(uv, vec2(-1,  0));
    vec3 colorM = fetchPixel(uv, vec2(0));
    m = luminance(colorM);

    // calculate contrast and skip if this pixel needs no anti-aliasing
    float highest;
    contrast = localContrast(highest);

    if (skip(highest)) {
        //color = vec3(0);
        color = colorM;
        return;
    }
    
    // fetch the remaining diagonal neighbors
    float ne = luminance(uv, vec2( 1,  1));
    float nw = luminance(uv, vec2(-1,  1));
    float se = luminance(uv, vec2( 1, -1));
    float sw = luminance(uv, vec2(-1, -1));

    float pixelBlend = pixelBlend();
    vec2 pixelStep;
    float edgeBlend = edgeBlend(pixelStep);

    float blend = max(pixelBlend, edgeBlend);
    

    // result
    color = fetchPixel(uv, pixelStep * blend);
    //color = vec3(edgeBlend - pixelBlend);
}
