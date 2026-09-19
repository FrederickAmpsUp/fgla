struct VertexInput {
    int id : SV_VertexID;
};

struct VertexOutput {
    float4 position : SV_Position;
    float3 color : COLOR;
};

[shader("vertex")]
VertexOutput vs_main(VertexInput input) {
    float2 positions[3] = float2[](
        float2(0.0,  -0.5),
        float2( 0.5,  0.5),
        float2(-0.5,  0.5)
    );

    float3 colors[3] = float3[](
        float3(1.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        float3(0.0, 0.0, 1.0)
    );

    VertexOutput out;
    out.position = float4(positions[input.id], 0.0, 1.0);
    out.color = colors[input.id];
    return out;
}

[shader("fragment")]
float4 fs_main(VertexOutput input) : SV_Target {
    return float4(input.color, 1.0);
}
