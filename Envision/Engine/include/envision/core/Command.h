#pragma once

#define REGISTER_COMMAND(className, commandType) className() : GPUCommand(commandType) {}

namespace env
{
	enum CommandType
	{
		UNKNOWN = 0,
		//BARRIER, // Is this necessary?
		DISPATCH,
		DRAW,
		UPLOAD,
	};


	struct GPUCommand
	{
		const CommandType Type = CommandType::UNKNOWN;

	protected:
		GPUCommand(const CommandType type) : Type(type) {}
	};

	struct DispatchCommand : public GPUCommand
	{
		REGISTER_COMMAND(DispatchCommand, CommandType::DISPATCH)
		int ThreadCountX = 0;
		int ThreadCountY = 0;
		int ThreadCountZ = 0;
	};
}