#pragma once

//using namespace CVD;

static double jitter = 0.0;

// Normalize entire image
template <class Rgbish, class Scalar> struct Normalize {
    static inline void convert(const Rgbish& from, Scalar& to) {
		int sum = from.red + from.green + from.blue;
		if (!sum) sum = 1;
		
		to.red = 255*from.red / sum;
		to.green = 255*from.green / sum;
		to.blue = 255*from.blue / sum;
	}
};

// Normalize single pixel into passed vars
template <class Rgbish> 
bool NormalizedRGB(Rgbish& from, float& r, float& g, float& b) {
	float sum = from.red + from.green + from.blue;
	if (!sum)
		return false;
	
	r=from.red / sum;
	g=from.green / sum;
	b=from.blue / sum;
	return true;
};

template <class Rgbish>
bool isSkinRCA1(Rgbish& pixel) {
	float r,g,b;
	if (NormalizedRGB(pixel, r,g,b)) {
		float sum = r+g+b;
		float sum_sq = sum*sum;
		
		if (	(r/g)			> (1.185-jitter)
			&&	(r*b) / sum_sq	> (0.107-jitter)
			&&	(r*g) / sum_sq	> (0.112-jitter))
			return true; // skin
	}
	return false; // not skin
}

template <class Rgbish>
bool isSkinRCA2(Rgbish& pixel) {
	float r,g,b;
	if (NormalizedRGB(pixel, r,g,b)) {
		float sum = r+g+b;
		
		if (	(b/g)				< (1.249+jitter)
			&&	(sum/(3*r))			> (0.696-jitter)
			&&	((1/3.0)-(b/sum))	> (0.014-jitter)
			&&	(g/(3*sum))			< (0.108+jitter))
			return true; // skin
	}
	return false;
}

template <class Rgbish>
bool isSkinRCA3(Rgbish& pixel) {
	float r,g,b;
	if (NormalizedRGB(pixel, r,g,b)) {
		float sum = r+g+b;
		
		if (	(3*b*r*r)					< (0.1276+jitter)
			&&	((sum/(3*r))+((r-g)/sum))	< (0.9498+jitter)
			&&	(((r*b)+(g*g)/(g*b))		< (2.7775+jitter)))
			return true; // skin
	}
	return false; // not skin
}

template <class Rgbish, class Scalar> struct SkinRCA1 {
    static inline void convert(const Rgbish& from, Scalar& to) {
		to = isSkinRCA1(from)? 255 : 0;
	}
};

template <class Rgbish, class Scalar> struct SkinRCA2 {
    static inline void convert(const Rgbish& from, Scalar& to) {
		to = isSkinRCA2(from)? 255 : 0;
	}
};

template <class Rgbish, class Scalar> struct SkinRCA3 {
    static inline void convert(const Rgbish& from, Scalar& to) {
		to = isSkinRCA3(from)? 255 : 0;
	}
};

template <class Rgbish, class Scalar> struct SkinRCA {
	static inline void convert(const Rgbish& from, Scalar& to) {
		CVD::byte scale_fac = 0;
		CVD::byte w1=1, w2=1, w3=1;
		//to = 0;
		
		if (isSkinRCA1(from))
			scale_fac+=w1;
		
		if (isSkinRCA2(from))
			scale_fac+=w2;
		
		if (isSkinRCA3(from))
			scale_fac+=w3;
		
		//		to = (from.red+from.green+from.blue)*scale_fac/3;
		to = 255*scale_fac/3;
	}
};

template <class Rgbish, class Scalar> struct SkinRCA_binary {
	static inline void convert(const Rgbish& from, Scalar& to) {
		bool isSkin = false;
		
		if (isSkinRCA1(from))
			isSkin = true;
		
		else if (isSkinRCA2(from))
			isSkin = true;
		
		else if (isSkinRCA3(from))
			isSkin = true;
		
		to = isSkin? 255 : 0;
	}
};

template <class Rgbish>
int isSkinRCA(Rgbish& pixel) {
	int score=0;
	float r,g,b;
	if (NormalizedRGB(pixel, r,g,b)) {
		float sum = r+g+b;
		float sum_sq = sum*sum;
		
		if (	(r/g)				> (1.185-jitter)
			&&	(r*b) / sum_sq		> (0.107-jitter)
			&&	(r*g) / sum_sq		> (0.112-jitter))
			score++; // method 1
		if (	(b/g)				< (1.249+jitter)
			&&	(sum/(3*r))			> (0.696-jitter)
			&&	((1/3.0)-(b/sum))	> (0.014-jitter)
			&&	(g/(3*sum))			< (0.108+jitter))
			score++; // method 2
		if (	(3*b*r*r)					< (0.1276+jitter)
			&&	((sum/(3*r))+((r-g)/sum))	< (0.9498+jitter)
			&&	(((r*b)+(g*g)/(g*b))		< (2.7775+jitter)))
			score++; // method 3
	}
	return score; // not skin
}

//template int isSkinRCA/*<Rgb<byte> >*/(Rgb<byte>&);

