#version 330 core
out vec4 FragColor;

in vec3 FragPos; // Vertex position in world space

void main() {
    // Set the water color to light blue 
    vec3 waterColor = vec3(0.0, 0.5, 0.5);

    float alpha = 0.7;

    FragColor = vec4(waterColor, alpha);
}
