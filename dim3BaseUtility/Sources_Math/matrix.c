/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Matrix Functions

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif
	
/* =======================================================

      Identity Matrix
      
======================================================= */
	
void matrix_identity(matrix_type *mat)
{
	memset(mat,0x0,sizeof(matrix_type));
	mat->data[0][0]=mat->data[1][1]=mat->data[2][2]=mat->data[3][3]=1;
}

/* =======================================================

      Multiple Matrix
      
======================================================= */

void matrix_multiply(matrix_type *mat_1,matrix_type *mat_2)
{
	int				i;
	matrix_type		mat;
	
	for (i=0;i!=4;i++) {
		mat.data[i][0]=
			(mat_1->data[i][0]*mat_2->data[0][0])+
			(mat_1->data[i][1]*mat_2->data[1][0])+
			(mat_1->data[i][2]*mat_2->data[2][0])+
			(mat_1->data[i][3]*mat_2->data[3][0]);

		mat.data[i][1]=
			(mat_1->data[i][0]*mat_2->data[0][1])+
			(mat_1->data[i][1]*mat_2->data[1][1])+
			(mat_1->data[i][2]*mat_2->data[2][1])+
			(mat_1->data[i][3]*mat_2->data[3][1]);

		mat.data[i][2]=
			(mat_1->data[i][0]*mat_2->data[0][2])+
			(mat_1->data[i][1]*mat_2->data[1][2])+
			(mat_1->data[i][2]*mat_2->data[2][2])+
			(mat_1->data[i][3]*mat_2->data[3][2]);

		mat.data[i][3]=
			(mat_1->data[i][0]*mat_2->data[0][3])+
			(mat_1->data[i][1]*mat_2->data[1][3])+
			(mat_1->data[i][2]*mat_2->data[2][3])+
			(mat_1->data[i][3]*mat_2->data[3][3]);
	}
	
	memmove(mat_1,&mat,sizeof(matrix_type));
}

void matrix_vertex_multiply(matrix_type *mat,float *x,float *y,float *z)
{
	float		fx,fy,fz;
	
	fx=*x;
	fy=*y;
	fz=*z;
	
	*x=(fx*mat->data[0][0])+(fy*mat->data[0][1])+(fz*mat->data[0][2])+mat->data[0][3];
	*y=(fx*mat->data[1][0])+(fy*mat->data[1][1])+(fz*mat->data[1][2])+mat->data[1][3];
	*z=(fx*mat->data[2][0])+(fy*mat->data[2][1])+(fz*mat->data[2][2])+mat->data[2][3];
}

void matrix_vertex_multiply_ignore_transform(matrix_type *mat,float *x,float *y,float *z)
{
	float		fx,fy,fz;
	
	fx=*x;
	fy=*y;
	fz=*z;
	
	*x=(fx*mat->data[0][0])+(fy*mat->data[0][1])+(fz*mat->data[0][2]);
	*y=(fx*mat->data[1][0])+(fy*mat->data[1][1])+(fz*mat->data[1][2]);
	*z=(fx*mat->data[2][0])+(fy*mat->data[2][1])+(fz*mat->data[2][2]);
}

void matrix_angle_multiply(matrix_type *mat,float *x,float *y,float *z)
{
	float		fx,fy,fz;
	
	fx=*x;
	fy=*y;
	fz=*z;
	
	*x=(fx*mat->data[0][0])+(fy*mat->data[0][1])+(fz*mat->data[0][2])+mat->data[0][3];
	*y=(fx*mat->data[1][0])+(fy*mat->data[1][1])+(fz*mat->data[1][2])+mat->data[1][3];
	*z=(fx*mat->data[2][0])+(fy*mat->data[2][1])+(fz*mat->data[2][2])+mat->data[2][3];
}

/* =======================================================

      Rotation Matrixes
      
======================================================= */

void matrix_rotate_x(matrix_type *mat,float ang)
{
	float		rad,r_cos,r_sin;
	
	rad=ang*ANG_to_RAD;
	r_cos=(float)cos(rad);
	r_sin=(float)sin(rad);
	
	matrix_identity(mat);

	mat->data[1][1]=r_cos;
	mat->data[1][2]=r_sin;
	mat->data[2][1]=-r_sin;
	mat->data[2][2]=r_cos;
}

void matrix_rotate_y(matrix_type *mat,float ang)
{
	float		rad,r_cos,r_sin;
	
	rad=ang*ANG_to_RAD;
	r_cos=(float)cos(rad);
	r_sin=(float)sin(rad);
	
	matrix_identity(mat);
	
	mat->data[0][0]=r_cos;
	mat->data[0][2]=-r_sin;
	mat->data[2][0]=r_sin;
	mat->data[2][2]=r_cos;
}

void matrix_rotate_z(matrix_type *mat,float ang)
{
	float		rad,r_cos,r_sin;
	
	rad=ang*ANG_to_RAD;
	r_cos=(float)cos(rad);
	r_sin=(float)sin(rad);
	
	matrix_identity(mat);
	
	mat->data[0][0]=r_cos;
	mat->data[0][1]=r_sin;
	mat->data[1][0]=-r_sin;
	mat->data[1][1]=r_cos;
}

void matrix_rotate_xyz(matrix_type *mat,float x_ang,float y_ang,float z_ang)
{
	matrix_type		y_mat,z_mat;
	
	matrix_rotate_x(mat,x_ang);

	matrix_rotate_y(&y_mat,y_ang);
	matrix_multiply(mat,&y_mat);
	
	matrix_rotate_z(&z_mat,z_ang);
	matrix_multiply(mat,&z_mat);
}

void matrix_rotate_zyx(matrix_type *mat,float x_ang,float y_ang,float z_ang)
{
	matrix_type		x_mat,y_mat;
	
	matrix_rotate_z(mat,z_ang);
	
	matrix_rotate_y(&y_mat,y_ang);
	matrix_multiply(mat,&y_mat);

	matrix_rotate_x(&x_mat,x_ang);
	matrix_multiply(mat,&x_mat);
}

void matrix_rotate_xzy(matrix_type *mat,float x_ang,float y_ang,float z_ang)
{
	matrix_type		y_mat,z_mat;
	
	matrix_rotate_x(mat,x_ang);
	
	matrix_rotate_z(&z_mat,z_ang);
	matrix_multiply(mat,&z_mat);

	matrix_rotate_y(&y_mat,y_ang);
	matrix_multiply(mat,&y_mat);
}

bool matrix_has_rotation(matrix_type *mat)
{
	if (mat->data[0][0]!=0.0f) return(TRUE);
	if (mat->data[0][1]!=0.0f) return(TRUE);
	if (mat->data[0][2]!=0.0f) return(TRUE);

	if (mat->data[1][0]!=0.0f) return(TRUE);
	if (mat->data[1][1]!=0.0f) return(TRUE);
	if (mat->data[1][2]!=0.0f) return(TRUE);

	if (mat->data[2][0]!=0.0f) return(TRUE);
	if (mat->data[2][1]!=0.0f) return(TRUE);
	if (mat->data[2][2]!=0.0f) return(TRUE);

	if (mat->data[3][0]!=0.0f) return(TRUE);
	if (mat->data[3][1]!=0.0f) return(TRUE);
	if (mat->data[3][2]!=0.0f) return(TRUE);

	return(FALSE);
}

/* =======================================================

      Translate Matrix
      
======================================================= */

void matrix_translate(matrix_type *mat,float x,float y,float z)
{
	matrix_identity(mat);
	
	mat->data[0][3]=x;
	mat->data[1][3]=y;
	mat->data[2][3]=z;
}

/* =======================================================

      Average Matrixes
      
======================================================= */

void matrix_average(matrix_type *mat,int nmatrix,matrix_type *mats)
{
	int				n,i;
	float			*f,*f2,f_count;

		// all zeros

	memset(mat,0x0,sizeof(matrix_type));

		// add up all the matrixes

	for (n=0;n!=nmatrix;n++) {
		f=(float*)mat->data;
		f2=(float*)mats[n].data;

		for (i=0;i!=16;i++) {
			*f=(*f)+(*f2++);
			f++;
		}
	}

		// average the matrixes

	f_count=(float)nmatrix;
	f=(float*)mat->data;

	for (i=0;i!=16;i++) {
		*f=(*f)/f_count;
		f++;
	}
}

