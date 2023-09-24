// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "DaylonUtils.h"

namespace Daylon
{
	class FAnimSpriteCel;

	typedef TFunction<void(FAnimSpriteCel&)> TAnimStartFunc;

	class DAYLONGRAPHICSLIBRARY_API FAnimSpriteCel : public Daylon::ImagePlayObject2D
	{
		public:

		FInt32Rect     SrcPx;
		FInt32Rect     DstPx;
		float          OriginalLifepsan;
		float          StartAge;
		bool           bAnimating = false;
		TAnimStartFunc AnimStartDelegate;


		void Init(
			const FSlateBrush& InBrush, 
			const FBox2d&      SrcUVs, 
			const FInt32Rect&  InSrcPx, 
			const FInt32Rect&  InDstPx, 
			float              InStartAge, 
			float              InAge, 
			TAnimStartFunc     InAnimStartDelegate);

		virtual void Update(float DeltaTime) override;
	};
}
