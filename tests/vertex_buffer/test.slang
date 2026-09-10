struct VertexInput {
    float2 position;
    float3 color;
};

struct VertexOutput {
    float4 position : SV_Position;
    float3 color : COLOR;
};

[shader("vertex")]
VertexOutput vs_main(VertexInput input) {
    VertexOutput out;
    out.position = float4(input.position, 0.0, 1.0);
    out.color = input.color;
    return out;
}

[shader("fragment")]
float4 fs_main(VertexOutput input) : SV_Target {
    return float4(input.color, 1.0);
}
