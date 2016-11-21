#Modified Files:
-raytrace.cpp:
explained in raytrace.cpp

-vecm.h:
from:	vec4 operator * ( const vec4& v ) const
	{ return vec4( x*v.x, y*v.y, z*v.z, w*v.z ); }
to:	vec4 operator * ( const vec4& v ) const
	{ return vec4( x*v.x, y*v.y, z*v.z, w*v.w ); }

#Test Passed: ALL
-testAmbient
-testBackground
-testBehind
-testDiffuse
-testIllum (ignored)
-testImgPlane
-testIntersection
-testParsing
-testReflection
-testSample
-testShadow
-testSpecular

#Test Failed: NONE