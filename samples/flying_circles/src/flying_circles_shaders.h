//
// Created by Alexander on 14.01.2026.
//

#pragma once


namespace flying_circles
{
	static inline const char* CircleVertexShader = R"(
	#version 330

	in vec3 vertexPosition;
	in vec2 vertexTexCoord;
	in vec4 vertexColor;

	uniform mat4 mvp;

	out vec2 fragTexCoord;
	out vec4 fragColor;

	void main()
	{
		fragTexCoord = vertexTexCoord;
		fragColor = vertexColor;
		gl_Position = mvp * vec4(vertexPosition, 1.0);
	}
	)";

	static inline const char*CircleFragmentShader = R"(
	#version 330

    in vec2 fragTexCoord;
    in vec4 fragColor;

    out vec4 finalColor;

    void main()
    {
        // Convert UV from [0,1] to [-1,1] centered coordinates
        vec2 uv = fragTexCoord * 2.0 - 1.0;

        // Calculate distance from center
        float dist = length(uv);

        // SDF circle: negative inside, positive outside
        float sdf = dist - 1.0;

        // Anti-aliased edge
        float edgeWidth = fwidth(dist) * 0.8;
        float alpha = 1.0 - smoothstep(-edgeWidth, edgeWidth, sdf);

        // Discard fully transparent pixels
        //if (alpha < 0.001) discard;

        finalColor = vec4(fragColor.rgb, fragColor.a * alpha);
    }
	)";




}