import Simple_shared;

struct VertexIn {
    float3 position;
    float3 color;
}

struct GlobalUBO {
    float4x4 view;
    float4x4 proj;
}

struct ModelConst {
    float4x4 model;
}


ConstantBuffer<GlobalUBO> globalUBO;

[vk::push_constant]
ConstantBuffer<ModelConst> modelConst;

[shader("vertex")]
VertexOutput vertMain(VertexIn vertIn) {
    VertexOutput output;
    output.color = vertIn.color;
    output.position = mul(globalUBO.proj, mul(globalUBO.view, mul(modelConst.model, float4(vertIn.position, 1.0))));
    return output;
}