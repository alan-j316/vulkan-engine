import Simple_shared;

struct VertexIn {
    float3 position;
    float3 color;
}

struct GlobalUBO {
    float4x4 view;
    float4x4 proj;
}

ConstantBuffer<GlobalUBO> globalUBO;

[shader("vertex")]
VertexOutput vertMain(VertexIn vertIn) {
    VertexOutput output;
    output.color = vertIn.color;
    output.position = mul(globalUBO.proj, mul(globalUBO.view, float4(vertIn.position, 1.0)));
    return output;
}