// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#include "DaylonPlayObject2D.h"
#include "DaylonWidgetUtils.h"
#include "DaylonLogging.h"



TSharedPtr<Daylon::ImagePlayObject2D> Daylon::SpawnImagePlayObject2D(const FSlateBrush& Brush, float Radius)
{
	auto PlayObj = SNew(ImagePlayObject2D);

	Daylon::Install<SImage>(PlayObj, Radius);

	PlayObj->SetBrush(Brush);

	return PlayObj;
}


TSharedPtr<Daylon::SpritePlayObject2D> Daylon::SpawnSpritePlayObject2D(const FDaylonSpriteAtlas& Atlas, const FVector2D& S, float Radius)
{
	auto PlayObj = SNew(SpritePlayObject2D);

	Daylon::Install<SDaylonSprite>(PlayObj, Radius);

	PlayObj->SetAtlas(Atlas);
	PlayObj->SetSize(S);
	PlayObj->UpdateWidgetSize();

	return PlayObj;
}


void Daylon::UninstallImpl(TSharedPtr<SWidget> Widget)
{
	Daylon::GetRootCanvas()->GetCanvasWidget()->RemoveSlot(Widget.ToSharedRef());
}

#define UNINSTALL_PLAYOBJECT(_Widget)	\
	if(!_Widget->IsValid())	\
	{	\
		UE_LOG(LogDaylon, Error, TEXT("Daylon::Destroy() tried to uninstall an invalid Slate widget!"));	\
		return;	\
	}	\
	_Widget->ClearSlot();	\
	UninstallImpl(StaticCastSharedPtr<SWidget>(Widget));


void Daylon::Uninstall(TSharedPtr<Daylon::ImagePlayObject2D> Widget)
{
	UNINSTALL_PLAYOBJECT(Widget);
}


void Daylon::Uninstall(TSharedPtr<SpritePlayObject2D> Widget)
{
	UNINSTALL_PLAYOBJECT(Widget);
}
