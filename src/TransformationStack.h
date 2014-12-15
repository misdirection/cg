#ifndef TRANSFORMATION_STACK_H
#define TRANSFORMATION_STACK_H

#include<stack>
#include <iostream>

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

class TransformationStack
{
public:
	TransformationStack();
	~TransformationStack();

	//multiply 
	void pushMatrix(glm::mat4 const& input_matrix);
	void popMatrix();
	glm::mat4 const topMatrix() const;

	void clear();

private:
	std::stack<glm::mat4> matrices;
};

#endif