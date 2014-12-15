#include <TransformationStack.h>

TransformationStack::TransformationStack()
{}

TransformationStack::~TransformationStack()
{}

void TransformationStack::pushMatrix(glm::mat4 const& input_matrix)
{
	if(matrices.empty())
	{
		matrices.push(input_matrix);
	}
	else
	{
		matrices.push(input_matrix * matrices.top() );
	}
}

void TransformationStack::popMatrix()
{
	if(!matrices.empty())
	{
		matrices.pop();
	}
	else
	{
		std::cerr << "Can not pop from empty transformation stack.\n";
	}
}

glm::mat4 const TransformationStack::topMatrix() const
{
	if(!matrices.empty())
	{
		return matrices.top();
	}
	else
	{
		return glm::mat4(1.0f);
	}
}

void TransformationStack::clear()
{
	while(!matrices.empty())
		matrices.pop();
}