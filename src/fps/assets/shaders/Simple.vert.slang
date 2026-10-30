import Simple_shared;

struct VertexIn {
    float3 position;
    float3 color;
}


[shader("vertex")]
VertexOutput vertMain(VertexIn vertIn) {
    VertexOutput output;
    output.color = vertIn.color;
    output.position = float4(vertIn.position, 1.0);
    return output;
}