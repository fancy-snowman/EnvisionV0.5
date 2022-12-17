#pragma once
#include "envision/graphics/FrameResource.h"
#include "envision/graphics/FramePassRecordContext.h"

#include <vector>

namespace env
{

	enum class PASS_TYPE
	{
		GRAPHICS,
		COMPUTE,
		COPY,
	};


	struct FramePass
	{
		std::string Name;
		PASS_TYPE Type;

		std::vector<FrameResource*> Inputs;
		std::vector<FrameResource*> Outputs;
		std::vector<FrameResource*> Samplers;

		FramePass(const std::string& name, PASS_TYPE type);
		virtual ~FramePass() = default;

		virtual void Record(FramePassRecordContext& context) = 0;
	};

	template <typename PassData, typename RecordFunc>
	struct TypedFramePass : public FramePass
	{
		PassData Data;
		RecordFunc DeferredRecord;

		TypedFramePass(const std::string& name, PASS_TYPE type, RecordFunc deferredRecord);
		~TypedFramePass() final = default;

		void Record(FramePassRecordContext& context) final;
	};

	template<typename PassData, typename RecordFunc>
	inline TypedFramePass<PassData, RecordFunc>::TypedFramePass(const std::string& name, PASS_TYPE type, RecordFunc deferredRecord)
		: FramePass(name, type), DeferredRecord(deferredRecord)
	{
		//
	}

	template<typename PassData, typename RecordFunc>
	inline void TypedFramePass<PassData, RecordFunc>::Record(FramePassRecordContext& context)
	{
		DeferredRecord(Data, context);
	}
}
