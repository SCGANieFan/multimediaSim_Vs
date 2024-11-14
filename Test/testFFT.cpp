#if 1
#include"testInner.h"

#if 0
typedef struct
{
	double real;
	double imag;
}Complex;

/* �����ӷ� */
void Complex_ADD(Complex a, Complex b, Complex* c)
{
	c->real = a.real + b.real;
	c->imag = a.imag + b.imag;
}
/* �������� */
void Complex_SUB(Complex a, Complex b, Complex* c)
{
	c->real = a.real - b.real;
	c->imag = a.imag - b.imag;
}
/* �����˷� */
void Complex_MUL(Complex a, Complex b, Complex* c)
{
	c->real = a.real * b.real - a.imag * b.imag;
	c->imag = a.real * b.imag + a.imag * b.real;
}
/* ��������ֵ */
double Complex_ABS(Complex* a)
{
	double b;
	b = sqrt((a->real) * (a->real) + (a->imag) * (a->imag));
	return b;
}

static double PI = 4.0 * atan(1); //����� ��Ϊtan(��/4)=1 ����arctan��1��*4=�У����Ӧеľ���


uint32_t FFT(uint32_t* signal_in, uint32_t signal_len, Complex* fft_out, uint32_t fft_point)
{
	LOG();
	Complex* W;//��ת����Wn
	uint32_t i, j, k, m;
	Complex temp1, temp2, temp3;//���ڽ����м���
	double series;//���м���
#if 1
	if (signal_len < fft_point)//�����������ź����ݶԱ� 
	{
		for (i = 0; i < signal_len; i++)
		{
			fft_out[i].real = signal_in[i];
			fft_out[i].imag = 0;
		}
		for (i = signal_len; i < fft_point; i++)//��0
		{
			fft_out[i].real = 0;
			fft_out[i].imag = 0;
		}
	}
	else if (signal_len == fft_point)
	{
		for (i = 0; i < signal_len; i++)
		{
			fft_out[i].real = signal_in[i];
			fft_out[i].imag = 0;
		}
	}
	else
	{
		for (i = 0; i < fft_point; i++)
		{
			fft_out[i].real = signal_in[i];
			fft_out[i].imag = 0;
		}
	}
#endif

	if (fft_point % 2 != 0)
	{
		return 1;
	}

	W = (Complex*)malloc(sizeof(Complex) * fft_point);
	if (W == NULL)
	{
		return 1;
	}

	for (i = 0; i < fft_point; i++)
	{
		W[i].real = cos(2 * PI / fft_point * i);	 //ŷ����ʾ��ʵ��
		W[i].imag = -1 * sin(2 * PI / fft_point * i);//ŷ����ʾ���鲿
	}
#if 1
	//������
	for (i = 0; i < fft_point; i++)
	{
		k = i;
		j = 0;
		series = log(fft_point) / log(2); //������еļ��� log2(fft_point)
		while ((series--) > 0)//���ð�λ���Լ�ѭ��ʵ����λ�ߵ�
		{
			j = j << 1;
			j |= (k & 1);
			k = k >> 1;
		}
		if (j > i) //��x(n)����λ����
		{
			temp1 = fft_out[i];
			fft_out[i] = fft_out[j];
			fft_out[j] = temp1;
		}
	}
#endif
	series = log(fft_point) / log(2); //��������ļ���

	for (i = 0; i < series; i++)
	{
		m = 1 << i; //��λ ÿ�ζ���2��ָ������ʽ���ӣ���ʵҲ������m=2^i����
		for (j = 0; j < fft_point; j += 2 * m) //һ��������㣬ÿһ��ĵ������ӳ�����ͬ
		{
			for (k = 0; k < m; k++)//���ν��ľ���  һ���������� ÿ�����ڵĵ�������
			{
				Complex_MUL(fft_out[k + j + m], W[fft_point * k / 2 / m], &temp1);
				Complex_ADD(fft_out[j + k], temp1, &temp2);
				Complex_SUB(fft_out[j + k], temp1, &temp3);
				fft_out[j + k] = temp2;
				fft_out[j + k + m] = temp3;
			}
		}
	}
	free(W);
	return 0;
}

#endif

#if 1
#include <stdio.h>  
#include <stdlib.h>  
#include <math.h>  

// ����PI  
#ifndef PI  
#define PI 3.14159265358979323846  
#endif  

// ���帴���ṹ��  
typedef struct {
	double real;
	double imag;
} complex_t;

// �������  
complex_t complex_add(complex_t a, complex_t b) {
	complex_t result;
	result.real = a.real + b.real;
	result.imag = a.imag + b.imag;
	return result;
}

// �������  
complex_t complex_sub(complex_t a, complex_t b) {
	complex_t result;
	result.real = a.real - b.real;
	result.imag = a.imag - b.imag;
	return result;
}

// ������ˣ�����������ʵ���ĳ˷���  
complex_t complex_mul(complex_t a, complex_t b) {
	complex_t result;
	result.real = a.real * b.real - a.imag * b.imag;
	result.imag = a.real * b.imag + a.imag * b.real;
	return result;
}

// ��������ʵ��  
complex_t complex_mul_real(complex_t a, double r) {
	complex_t result;
	result.real = a.real * r;
	result.imag = a.imag * r;
	return result;
}

// ������������  
void swap(complex_t* a, complex_t* b) {
	complex_t temp = *a;
	*a = *b;
	*b = temp;
}

// ����FFT  
void fft(complex_t* X, int n) {
	if (n <= 1) return;

	// �����ڴ��ż��������������Ԫ��  
	complex_t* X_even = (complex_t*)malloc(n / 2 * sizeof(complex_t));
	complex_t* X_odd = (complex_t*)malloc(n / 2 * sizeof(complex_t));
	for (int i = 0; i < n / 2; i++) {
		X_even[i] = X[i * 2];
		X_odd[i] = X[i * 2 + 1];
	}

	// �ݹ����FFT  
	fft(X_even, n / 2);
	fft(X_odd, n / 2);

	// Ԥ������ת����  
	for (int k = 0; k < n / 2; k++) {
		double angle = -2 * PI * k / n;
		complex_t w = { cos(angle), sin(angle) };
		complex_t t = complex_mul(w, X_odd[k]);

		// ��Ͻ��  
		X[k] = complex_add(X_even[k], t);
		X[k + n / 2] = complex_sub(X_even[k], t);
	}

	// �ͷ��ڴ�  
	free(X_even);
	free(X_odd);
}

// ��ӡ��������  
void print_complex_array(complex_t* X, int n) {
	for (int i = 0; i < n; i++) {
		printf("%.5f + %.5fi\n", X[i].real, X[i].imag);
	}
}


#endif

void testFFT()
{
	LOG();
#if 0
	const uint32_t signal_len = 4;
	uint32_t signal_in[signal_len];
	signal_in[0] = 0;
	signal_in[1] = 1;
	signal_in[2] = 0;
	signal_in[3] = 1;
	const uint32_t fft_point = signal_len;
	Complex fft_out[fft_point];
	FFT(signal_in, signal_len, fft_out, fft_point);
#endif

#if 1
	// ʾ�����ݣ�һ������8������������  
	complex_t X[] = {
		{0.0, 0.0}, 
		{1.0, 0.0},
		{0.0, 0.0}, 
		{1.0, 0.0},
		{1.0, 0.0}
	};

	int n = sizeof(X) / sizeof(X[0]);

	// ��ӡԭʼ����  
	printf("Original data:\n");
	print_complex_array(X, n);

	// ����FFT  
	fft(X, n);

	// ��ӡFFT���  
	printf("\nFFT result:\n");
	print_complex_array(X, n);
#endif
	while (1);
}

#endif