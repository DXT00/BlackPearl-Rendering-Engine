#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
void main(){
	
	gl_Position = vec4(aPos,1.0);

}
#type fragment
#version 430 core

const uint STACK_SIZE = 23u;
#define OFFSET 3.552713678800501e-15

layout(rgba32f,binding = 0) uniform image2D u_Color;
layout(rgba8,binding = 1) uniform image2D u_Albedo;
layout(rgba8_snorm,binding = 2) uniform image2D u_Normal;


uniform layout(binding = 3,r32ui) uimageBuffer u_octreeBuffer;
uniform layout(binding = 4,r32ui) uimageBuffer u_octreeKd;
layout(std430,binding = 5) readonly buffer SobolSSBO{vec2 u_SobolSSBO[];};


uniform int  u_octreeLevel;
uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_TranInverseModel;
uniform vec3 u_CameraViewPos;
uniform int  u_ViewType;
uniform int  u_SPP;
uniform int  u_ScreenWidth;
uniform int  u_ScreenHeight;
uniform int  u_Bounce;
uniform vec3 u_SunRadiance;

out vec4 FragColor;

struct StackItem{
	uint  node;
	float t_max;
}stack[STACK_SIZE];

vec4 convRGBA8ToVec4( in uint val )
{
    return vec4( float( (val&0x000000FFu) ), float( (val&0x0000FF00u)>>8u),
	             float( (val&0x00FF0000u)>>16u), float( (val&0xFF000000u)>>24u) );
}

//半球坐标转换到fragment坐标系
vec3 AlignDirection(in const vec3 dir,in const vec3 target){
	vec3 u = normalize(cross(abs(target.x)>0.01?vec3(0,1,0):vec3(1,0,0) ,target ));
	vec3 v = cross(target,u);
	return dir.x*u+dir.y*v+dir.z*target;
}
#define TWO_PI 6.28318530718f

vec3 SampleHemishpere(vec2 r,in const float e){
	r.x *= TWO_PI;
	float cos_phi = cos(r.x);
	float sin_phi = sin(r.x);
	float cos_theta = pow((1.0f - r.y), 1.0f/(e + 1.0f));
	float sin_theta = sqrt(1.0f - cos_theta*cos_theta);
	float pu = sin_theta * cos_phi;
	float pv = sin_theta * sin_phi;
	float pw = cos_theta;
	return normalize(vec3(pu, pv, pw)); 

}
bool RayMarchLeaf(vec3 o,vec3 d,out vec3 o_pos,out vec3 o_color,out vec3 o_normal){
	
	d.x = abs(d.x) >= OFFSET ? d.x : (d.x >= 0 ? OFFSET : -OFFSET);
	d.y = abs(d.y) >= OFFSET ? d.y : (d.y >= 0 ? OFFSET : -OFFSET);
	d.z = abs(d.z) >= OFFSET ? d.z : (d.z >= 0 ? OFFSET : -OFFSET);

	// Precompute the coefficients of tx(x), ty(y), and tz(z).
	// The octree is assumed to reside at coordinates [1, 2].
	vec3 t_coef = 1.0f / -abs(d);
	vec3 t_bias = t_coef * o;

	uint oct_mask = 0u;
	
	//要改
	if(d.x>0.0f) oct_mask^=1u, t_bias.x = 3.0f * t_coef.x - t_bias.x;
	if(d.y>0.0f) oct_mask^=2u, t_bias.z = 3.0f * t_coef.z - t_bias.z;
	if(d.z>0.0f) oct_mask^=4u, t_bias.y = 3.0f * t_coef.y - t_bias.y; 
//	if(d.x>0.0f) oct_mask^=1u, t_bias.x = 1.0f * t_coef.x - t_bias.x;
//	if(d.z>0.0f) oct_mask^=2u, t_bias.z = 1.0f * t_coef.z - t_bias.z;
//	if(d.y>0.0f) oct_mask^=4u, t_bias.y = 1.0f * t_coef.y - t_bias.y; 

//	float t_min = max(max(1.0f * t_coef.x - t_bias.x,1.0f * t_coef.y - t_bias.y),1.0f * t_coef.z - t_bias.z);
//	float t_max = max(max(0.0f * t_coef.x - t_bias.x,0.0f * t_coef.y - t_bias.y),0.0f *t_coef.z - t_bias.z);
	float t_min = max(max(2.0f * t_coef.x - t_bias.x,2.0f * t_coef.y - t_bias.y),2.0f * t_coef.z - t_bias.z);
	float t_max = max(max(1.0f * t_coef.x - t_bias.x,1.0f * t_coef.y - t_bias.y),1.0f *t_coef.z - t_bias.z);
	t_min = max(t_min,0.0f);
	float h = t_max;

	uint parent = imageLoad(u_octreeBuffer,0).r;//root = 0
	if((parent & 0x80000000u) ==0u)return false;
	parent &=0x3fffffffu;
	uint cur = 0u;
	vec3 pos = vec3(1.0f);
	uint idx = 0u;
	
	int leafIdx;

	if(1.5f * t_coef.x - t_bias.x > t_min) idx ^= 1u, pos.x = 1.5f;
	if(1.5f * t_coef.z - t_bias.z > t_min) idx ^= 2u, pos.z = 1.5f;
	if(1.5f * t_coef.y - t_bias.y > t_min) idx ^= 4u, pos.y = 1.5f;
	
	uint scale = STACK_SIZE-1u;
	uint scale_flag;
	float scale_exp2 = 0.5f;
	while(scale<STACK_SIZE){
		//++iter;
		if(cur==0u) 
			cur = imageLoad(u_octreeBuffer,int(parent+(idx^oct_mask))).r;  
			///+ 运算符是优先于 ^ 的！
			//cur = imageLoad(u_octreeBuffer,int(parent+idx^oct_mask)).r;
		vec3 t_corner = pos*t_coef-t_bias;
		float tc_max = min(min(t_corner.x,t_corner.y),t_corner.z);
		//imageStore(u_debugBuffer,int(gl_FragCoord.y*540+gl_FragCoord.x),uvec4(tc_max,t_max,0u,0u));

		if((cur&0x80000000u)!=0u && t_min<=t_max){
			//INTERSECT
			float tv_max = min(t_max,tc_max);
			float half_scale_exp2 = scale_exp2*0.5f;
			vec3 t_center = half_scale_exp2*t_coef+ t_corner;

			if(t_min <= tv_max){
				if((cur & 0x40000000u)!=0u){
					//leaf node
					leafIdx = int(parent+(idx^oct_mask));
					//leafIdx = int(parent+idx^oct_mask);
					break;
				}
					

				//PUSH 
				if(tc_max<h){
					stack[scale].node = parent;
					stack[scale].t_max = t_max;
					
				}
				h = tc_max;

				parent = (cur& 0x3fffffffu);

				idx = 0u;
				-- scale;
				scale_flag = scale;
				scale_exp2 = half_scale_exp2;

				if(t_center.x > t_min) idx ^= 1u, pos.x += scale_exp2;
				if(t_center.z > t_min) idx ^= 2u, pos.z += scale_exp2;
				if(t_center.y > t_min) idx ^= 4u, pos.y += scale_exp2;

				cur = 0u;
				t_max = tv_max;

				continue;
			}//if(t_min<tv_max)

		}//if((cur&0x80000000u)!=0u && t_min<=t_max)

		//ADVANCE
		uint step_mask = 0u;
		if(t_corner.x <= tc_max) step_mask ^= 1u, pos.x -= scale_exp2;
		if(t_corner.z <= tc_max) step_mask ^= 2u, pos.z -= scale_exp2;
		if(t_corner.y <= tc_max) step_mask ^= 4u, pos.y -= scale_exp2;

		// Update active t-span and flip bits of the child slot index.
		t_min = tc_max;
		idx ^= step_mask;

		// Proceed with pop if the bit flips disagree with the ray direction.
		if( (idx & step_mask) != 0u ) {
			// POP
			// Find the highest differing bit between the two positions.
			uint differing_bits = 0u;
			if ((step_mask & 1u) != 0u) differing_bits |= floatBitsToUint(pos.x) ^ floatBitsToUint(pos.x + scale_exp2);
			if ((step_mask & 2u) != 0u) differing_bits |= floatBitsToUint(pos.z) ^ floatBitsToUint(pos.z + scale_exp2);
			if ((step_mask & 4u) != 0u) differing_bits |= floatBitsToUint(pos.y) ^ floatBitsToUint(pos.y + scale_exp2);
			scale = findMSB(differing_bits);
			scale_exp2 = uintBitsToFloat((scale - (STACK_SIZE) + 127u) << 23u); // exp2f(scale - s_max)

			// Restore parent voxel from the stack.
			parent = stack[scale].node;
			t_max  = stack[scale].t_max;

			// Round cube position and extract child slot index.
			uint shx = floatBitsToUint(pos.x) >> scale;
			uint shy = floatBitsToUint(pos.y) >> scale;
			uint shz = floatBitsToUint(pos.z) >> scale;
			pos.x = uintBitsToFloat(shx << scale);
			pos.y = uintBitsToFloat(shy << scale);
			pos.z = uintBitsToFloat(shz << scale);
			idx  = (shx & 1u) | ((shz & 1u) << 1u) | ((shy & 1u) << 2u);

			// Prevent same parent from being stored again and invalidate cached child descriptor.
			h = 0.0f;
			cur = 0u;
		}
	}//while(scale<STACK_SIZE)

	vec3 norm,t_corner = t_coef*(pos+scale_exp2)-t_bias;
	if(t_corner.x>t_corner.y && t_corner.x>t_corner.z)
		norm = vec3(-1,0,0);
	else if(t_corner.y>t_corner.z)
		norm = vec3(0,-1,0);
	else
		norm = vec3(0,0,-1);
	if((oct_mask & 1u)==0u ) norm.x = -norm.x;
	if((oct_mask & 2u)==0u ) norm.z = -norm.z;
	if((oct_mask & 4u)==0u ) norm.y = -norm.y;


	// Undo mirroring of the coordinate system.
	if ((oct_mask & 1u) != 0u) pos.x = 3.0f - scale_exp2 - pos.x;
	if ((oct_mask & 2u) != 0u) pos.z = 3.0f - scale_exp2 - pos.z;
	if ((oct_mask & 4u) != 0u) pos.y = 3.0f - scale_exp2 - pos.y;

	//Output result
	o_pos = clamp(o+t_min*d,pos,pos+scale_exp2);
	if(norm.x != 0) o_pos.x = norm.x > 0 ? pos.x + scale_exp2 + OFFSET*2 : pos.x - OFFSET;
	if(norm.y != 0) o_pos.y = norm.y > 0 ? pos.y + scale_exp2 + OFFSET*2 : pos.y - OFFSET;
	if(norm.z != 0) o_pos.z = norm.z > 0 ? pos.z + scale_exp2 + OFFSET*2 : pos.z - OFFSET;

	o_normal = norm;
	uint color =imageLoad(u_octreeKd,leafIdx).r;


	o_color = convRGBA8ToVec4(color).rgb/255.0;
	//vec3( cur & 0xffu, (cur >> 8u) & 0xffu, (cur >> 16u) & 0xffu) * 0.00392156862745098f;
	//o_t = t_min;

	return scale<STACK_SIZE && t_min<=t_max;

}


vec3 GenRay(){
	vec2 texcoord = vec2(gl_FragCoord.xy)/vec2(u_ScreenWidth,u_ScreenHeight);
	texcoord = 2.0*texcoord - 1.0;
	return normalize(  mat3(inverse(u_View))*( inverse(u_Projection) * vec4(texcoord, 1, 1) ).xyz  );
}

void main(){
	ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
	pixelCoord.y = u_ScreenHeight - 1 -pixelCoord.y;
	//pause
//	if(u_SPP == -1){
//		//color
//		if(u_ViewType==0){
//			vec3 pt_color = imageLoad(u_Color,pixelCoord).xyz;
//			if(pt_color.x ==0.0&&pt_color.y==0.0&&pt_color.z==0.0)
//				FragColor = vec4(1,0,0,1);
//			else
//				FragColor = vec4(pow(pt_color, vec3(1.0f / 2.2f)), 1);
//		}
//		//albedo
//		else if(u_ViewType == 1) {
//			vec3 pt_albedo = imageLoad(u_Albedo, pixelCoord).xyz;
//			FragColor = vec4(pt_albedo, 1);
//		}
//		//normal
//		else {
//			vec3 pt_normal = imageLoad(u_Normal, pixelCoord).xyz;
//			FragColor = vec4(pt_normal * 0.5f + 0.5f, 1);
//		}
//		return;		
//
//	}
	//else{
		vec3 o = u_CameraViewPos;
		vec3 d = GenRay();

		vec3 acc_color = vec3(1);//vec3(1);
		vec3 indirect_color = vec3(0);
		vec3 radiance  = vec3(0);
		float attenuation = 0.5;
		vec3 pos,albedo,normal;
		vec3 direct_albedo = vec3(0.0);
		vec3 direct_normal = vec3(0.0);
		int hitNumber = 0;

		if(RayMarchLeaf(o,d,pos,albedo,normal)){
			radiance += albedo;
			direct_albedo = albedo;
			direct_normal = normal;

			//fract(x) 返回x的小数部分 
			for(int cur=0;cur<u_Bounce;cur++){
				d = AlignDirection(SampleHemishpere(fract(u_SobolSSBO[cur]),0.0f),normal);
				o = pos;

				if(RayMarchLeaf(o,d,pos,albedo,normal)){
					//hitNumber++;
					radiance+= (albedo* attenuation);
					attenuation*=0.5;
					//acc_color *= albedo;
				}
				else{
					break;
				}
			}
			//radiance *= u_SunRadiance;


		}
		else{
			radiance = u_SunRadiance;
		}
		//acc_color+=indirect_color;
		//acc_color = pow(acc_color, vec3(1.0f / 2.2f));
		//vec3 pt_color = (imageLoad(u_Color, pixelCoord).xyz * u_SPP + radiance) / float(u_SPP + 1);
		//imageStore(u_Color, pixelCoord, vec4(pt_color,1));
//
//		vec3 pt_albedo = (imageLoad(u_Albedo, pixelCoord).xyz * u_SPP + direct_albedo) / float(u_SPP + 1);
//		imageStore(u_Albedo, pixelCoord, vec4(pt_albedo, 1));
//
//		vec3 pt_normal = (imageLoad(u_Normal, pixelCoord).xyz * u_SPP + direct_normal) / float(u_SPP + 1);
//		imageStore(u_Normal, pixelCoord, vec4(pt_normal, 1));
		
		//vec3 pt_color = acc_color;
		if(u_ViewType == 0) //color
			//FragColor = vec4(radiance, 1);
			FragColor = vec4(pow(radiance, vec3(1.0f / 2.2f)), 1);
//		else if(u_ViewType == 1) //albedo
//			FragColor = vec4(pt_albedo, 1);
//		else //normal
//			FragColor = vec4(pt_normal * 0.5f + 0.5f, 1);
	//}
	
}