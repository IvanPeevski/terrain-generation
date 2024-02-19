#version 330 core
out vec4 FragColor;

in float height; // Height from vertex shader

void main()
{
    vec3 sandColor = vec3(0.76, 0.70, 0.50); // Light brown color for sand
    vec3 grassColor = vec3(0.13, 0.55, 0.13); // Green color for grass
    vec3 stoneColor = vec3(0.5, 0.5, 0.5); // Grey color for stone
    vec3 snowColor = vec3(1.0, 1.0, 1.0); // White color for snow
    
    // Interpolate between colors based on height
    if (height < 0.3) {
        FragColor = vec4(mix(sandColor, grassColor, height / 0.3), 1.0);
    } else if (height < 0.6) {
        FragColor = vec4(mix(grassColor, stoneColor, (height - 0.3) / 0.3), 1.0);
    } else {
        FragColor = vec4(mix(stoneColor, snowColor, (height - 0.6) / 0.4), 1.0);
    }
}