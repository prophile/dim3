/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script C<-->JSVAL Conversion

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"

extern js_type			js;

JSClass			d3_ang_class={"d3_ang_class",0,
							JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};
JSClass			d3_point_class={"d3_point_class",0,
							JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

/* =======================================================

      Float-JSVAL Translations
      
======================================================= */

float script_value_to_float(jsval val)
{
    jsdouble		jd;
    
	JS_ValueToNumber(js.cx,val,&jd);
	return((float)jd);
}

jsval script_float_to_value(float f)
{
    jsval		vp;
    
    JS_NewDoubleValue(js.cx,(jsdouble)f,&vp);
    return(vp);
}

/* =======================================================

      String-JSVAL Translations
      
======================================================= */

void script_value_to_string(jsval val,char *str,int len)
{
	JSString		*jstr;

	if (val==JSVAL_NULL) {
		str[0]=0x0;
		return;
	}
	
	jstr=JS_ValueToString(js.cx,val);
	strncpy(str,JS_GetStringBytes(jstr),len);
	str[len-1]=0x0;
}

jsval script_string_to_value(char *str)
{
	JSString		*jstr;
	
	jstr=JS_NewStringCopyZ(js.cx,str);
	return(STRING_TO_JSVAL(jstr));
}

/* =======================================================

      Array-JSVAL Translations
      
======================================================= */

jsval script_int_array_to_value(int cnt,int *values)
{
	int				n;
	JSObject		*j_obj;
	jsval			j_vals[64];

		// 64 is the longest array

	if (cnt>64) cnt=64;

		// create values

	for (n=0;n!=cnt;n++) {
		j_vals[n]=INT_TO_JSVAL(values[n]);
	}

		// create array

	j_obj=JS_NewArrayObject(js.cx,JSVAL_TO_INT(cnt),j_vals);
	return(OBJECT_TO_JSVAL(j_obj));
}

/* =======================================================

      Create dim3 Primitives
      
======================================================= */

jsval script_angle_to_value(float x,float y,float z)
{
	JSObject		*j_obj;

	j_obj=JS_NewObject(js.cx,&d3_ang_class,NULL,NULL);
	if (j_obj==NULL) return(JSVAL_NULL);

	JS_DefineProperty(js.cx,j_obj,"x",script_float_to_value(x),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"y",script_float_to_value(y),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"z",script_float_to_value(z),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	
	return(OBJECT_TO_JSVAL(j_obj));
}

jsval script_point_to_value(int x,int y,int z)
{
	JSObject		*j_obj;

	j_obj=JS_NewObject(js.cx,&d3_point_class,NULL,NULL);
	if (j_obj==NULL) return(JSVAL_NULL);

	JS_DefineProperty(js.cx,j_obj,"x",INT_TO_JSVAL(x),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"y",INT_TO_JSVAL(y),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"z",INT_TO_JSVAL(z),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	
	return(OBJECT_TO_JSVAL(j_obj));
}
