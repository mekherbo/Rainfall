#ifndef N_HPP
#define N_HPP

class N {
	private:
		int value;
		char annotation[100];
	public:
		N(int value);
		virtual ~N() = default;
		void setAnnotation(char* param_1);
		virtual int operator+(N& param_1);
		virtual int operator-(N& param_1);
};


#endif