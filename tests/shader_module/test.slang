import util;

struct VertexInput {
    float3 position : POSITION;
};

struct VertexOutput {
    float4 position : SV_Position;
}

[shader("vertex")]
VertexOutput vertex_main(VertexInput input) {
    VertexOutput output;

    output.position = foo(input.position);

    return output;
}
