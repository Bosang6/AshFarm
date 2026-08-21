#pragma once

#include "CoreMinimal.h"

namespace PlantBedDefaults
{
	static constexpr float DEFAULT_SOIL_FERTILITY				     = 60.0f;   //默认土壤肥力
	static constexpr float DEFAULT_SOIL_MAX_FERTILITY				 = 100.0f;  //默认最大土壤肥力

	static constexpr float FERTILITY_SALINE_THRESHOLD		    	 = 10.0f;   //肥力<10时土壤盐碱化，生长速度为0（彻底不长）
	static constexpr float FERTILITY_POOR_THRESHOLD		    		 = 30.0f;   //土壤肥力低于此阈值为 贫瘠
	static constexpr float FERTILITY_FERTILE_THRESHOLD		    	 = 70.0f;   //土壤肥力低于此阈值为 肥沃
	
	static constexpr float FERTILITY_LOSS_PER_RADIATION		    	 = 0.01f;   //辐射等级乘数, 土壤肥力损失量
	static constexpr float FERTILITY_LOSS_PER_SECOND		    	 = 0.01f;   //土壤肥力自然流失量
	static constexpr float FERTILITY_RECOVER_PER_SECOND		    	 = 0.2f;    //当辐射等级为0时,土壤肥力自然恢复
	
	static constexpr float MOISTURE_LOSS_PER_SECOND		    	     = 0.5f;   //土壤水分自然流失量
	static constexpr float MOISTURE_SALINE_LOSS_MULTI			     = 1.5f;     //乘数, 盐碱地土壤水分流失
	static constexpr float MOISTURE_POOR_LOSS_MULTI		  		     = 2.5f;     //乘数, 贫瘠土壤水分流失
	static constexpr float MOISTURE_NORMAL_LOSS_MULTI			     = 1.0f;     //乘数, 贫瘠土壤水分流失
	static constexpr float MOISTURE_FERTILE_LOSS_MULTI		 	     = 0.5f;     //乘数, 贫瘠土壤水分流失

	static constexpr float FERTILITY_SAND_LOSS_MULTI		    	 = 1.5f;   //乘数, 沙土土壤肥力流失
	static constexpr float FERTILITY_LOAM_LOSS_MULTI		    	 = 1.0f;   //乘数, 泥土土壤肥力流失
	static constexpr float FERTILITY_CLAY_LOSS_MULTI		    	 = 0.7f;   //乘数, 黏土土壤肥力流失

	static constexpr float MOISTURE_SAND_LOSS_MULTI			    	 = 1.5f;   //乘数, 沙土土壤湿度流失
	static constexpr float MOISTURE_LOAM_LOSS_MULTI		   		 	 = 1.0f;   //乘数, 泥土土壤湿度流失
	static constexpr float MOISTURE_CLAY_LOSS_MULTI		  		  	 = 0.7f;   //乘数, 黏土土壤湿度流失
} 

// 土壤品质
UENUM(BlueprintType)
enum class ESoilQuality : uint8
{
	Poor    UMETA(DisplayName = "贫瘠"),
	Normal  UMETA(DisplayName = "正常"),
	Fertile UMETA(DisplayName = "肥沃"),
	Saline  UMETA(DisplayName = "盐碱地"),
};

// 土壤类型
UENUM(BlueprintType)
enum class ESoilType : uint8
{
	Sand 	UMETA(DisplayName = "沙土"),
	Loam 	UMETA(DisplayName = "泥土"),
	Clay 	UMETA(DisplayName = "黏土"),
};

