
var glSystemConfig = [];

//
// start aload
//
load_current_config();
load_system_config();
load_system_status();

function load_current_config()
{
    if ( document.getElementById("Local_Project_Name") != null )
    {
        document.getElementById("Local_Project_Name").value = "Loading...";
        document.getElementById("Local_Project_Desc").value = "Loading...";
        // document.getElementById("WGS48OriginX").value = "Loading...";
        // document.getElementById("WGS48OriginY").value = "Loading...";
        // document.getElementById("MAP_North").value = "Loading...";
        // document.getElementById("BuildingOriginX").value = "Loading...";
        // document.getElementById("BuildingOriginY").value = "Loading...";
        // document.getElementById("BuildingXLength").value = "Loading...";
        // document.getElementById("BuildingYLength").value = "Loading...";
        // document.getElementById("BuildingID").value = "Loading...";
        // document.getElementById("BuildingFloor").value = "Loading...";
        document.getElementById("prjsavebtn").disabled = true;
    }

    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadconfig=1'+'&project_id='+project_id , function(data)
    {
        if ( document.getElementById("Local_Project_Name") != null )
        {
            document.getElementById("Local_Project_Name").value = data.PROJECT_NAME;
            document.getElementById("Local_Project_Desc").value = data.PROJECT_DESCRIPTION;
            // document.getElementById("WGS48OriginX").value = data.WGS48OriginX;
            // document.getElementById("WGS48OriginY").value = data.WGS48OriginY;
            // document.getElementById("MAP_North").value = data.MAP_North;
            // document.getElementById("BuildingOriginX").value = data.BuildingOriginX;
            // document.getElementById("BuildingOriginY").value = data.BuildingOriginY;
            // document.getElementById("BuildingXLength").value = data.BuildingXLength;
            // document.getElementById("BuildingYLength").value = data.BuildingYLength;
            // document.getElementById("BuildingID").value = data.BuildingID;
            // document.getElementById("BuildingFloor").value = data.BuildingFloor;
            document.getElementById("prjsavebtn").disabled = false;
        }

    });
}

function load_system_config()
{
    if ( document.getElementById("systemconfigformid") != null )
    {
        document.getElementById("CenterWebPortInputText").value = "Loading...";
        document.getElementById("CenterUdpPortInputText").value = "Loading...";
        document.getElementById("CoordUdpPortInputText").value = "Loading...";
        document.getElementById("CenterBroadcastPortInputText").value = "Loading...";
        document.getElementById("CenterWebSocketPortInputText").value = "Loading...";
        document.getElementById("TagAlertTimeoutInputText").value = "Loading...";
        document.getElementById("TagVoltageLowInputText").value = "Loading...";

        document.getElementById("VILSServerInputText").value = "Loading...";
        document.getElementById("ERPServerInputText").value = "Loading...";
        document.getElementById("SSOIPInputText").value = "Loading...";
        // document.getElementById("PushNotifyPrefixInputText").value = "Loading...";
        // document.getElementById('EnablePushNotifyCheckBtn').checked = false;
        document.getElementById("systemconfigupdatebtn").disabled = true;
    }

    if ( document.getElementById("algorithmconfigformid") != null )
    {
        document.getElementById("ValidDataTimeInputText").value = "Loading...";
        document.getElementById("MaxRangingDistanceInputText").value = "Loading...";
        document.getElementById('NoiseMakupCheckBtn').checked = false;
        document.getElementById("NoiseMakupRatioInputText").value = "Loading...";
        document.getElementById("NoiseMakupWeightInputText").value = "Loading...";
        document.getElementById('EnableKalman1DCheckBtn').checked = false;
        document.getElementById('EnableRangingReliabilityCheckBtn').checked = false;
        document.getElementById("RangingReliabilityMaxDistanceInputText").value = "Loading...";
        document.getElementById("RangingReliabilityMinCountInputText").value = "Loading...";
        document.getElementById("RangingReliabilityAverageDiffInputText").value = "Loading...";

        document.getElementById('Anchor1DMappingCheckBtn').checked = false;
        document.getElementById('OneAnchorCalculationCheckBtn').checked = false;
        document.getElementById('GDOPresolveCheckBtn').checked = false;
        document.getElementById("GDOPThresholdInputText").value = "Loading...";
        document.getElementById('IMUVelocityCheckBtn').checked = false;
        document.getElementById("IMUVelocityOffsetInputText").value = "Loading...";
        document.getElementById("SteadyStateOffsetInputText").value = "Loading...";
        document.getElementById("StayInWallTimeoutInputText").value = "Loading...";
        document.getElementById('EnableFixHeightCheckBtn').checked = false;

        document.getElementById('optionsPositionAlgorithm1').checked = false;
        document.getElementById('optionsPositionAlgorithm2').checked = false;
        document.getElementById('optionsPositionAlgorithm3').checked = false;
        document.getElementById("WeightMovingAverageInputText1").value = "Loading...";
        document.getElementById("WeightMovingAverageInputText2").value = "Loading...";
        document.getElementById("WeightMovingAverageInputText3").value = "Loading...";

        document.getElementById('SmoothingResultCheckBtn').checked = false;
        document.getElementById("SmoothingResultOffsetInputText").value = "Loading...";
        document.getElementById("SmoothingResultStepInputText").value = "Loading...";
        document.getElementById("SmoothingResultFirstStateOffsetRatioInputText").value = "Loading...";
        document.getElementById("SmoothingResultFirstStateRunTimeInputText").value = "Loading...";
        document.getElementById("SmoothingResultSecondStateOffsetRatioInputText").value = "Loading...";
        document.getElementById("SmoothingResultSecondStateRunTimeInputText").value = "Loading...";

        document.getElementById("algorithmconfigupdatebtn").disabled = true;
    }

    if ( document.getElementById("lorawanconfigformid") != null )
    {
        document.getElementById("LoRAWANTPOICInputText").value = "Loading...";
        document.getElementById("MQTTHostIpInputText").value = "Loading...";
        document.getElementById("MQTTHostPortInputText").value = "Loading...";
        document.getElementById("MQTTHostUsernameInputText").value = "Loading...";
        document.getElementById("MQTTHostPasswordInputText").value = "Loading...";
        document.getElementById("MQTTClientIdInputText").value = "Loading...";

        document.getElementById("lorawanconfigupdatebtn").disabled = true;
    }

    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {

        console.log(data);

        if ( document.getElementById("systemconfigformid") != null )
        {
            document.getElementById("CenterWebPortInputText").value = data.web_udp_port;
            document.getElementById("CenterUdpPortInputText").value = data.center_udp_port;
            document.getElementById("CoordUdpPortInputText").value = data.coord_udp_port;
            document.getElementById("CenterBroadcastPortInputText").value = data.center_broadcast_port;
            document.getElementById("CenterWebSocketPortInputText").value = data.web_socket_port;
            document.getElementById("TagAlertTimeoutInputText").value = data.TagAlertTimeout;
            document.getElementById("TagVoltageLowInputText").value = data.TagAlertVoltageLow;
            document.getElementById("VILSServerInputText").value = data.VILSServer;
            document.getElementById("ERPServerInputText").value = data.ERPServer;
            document.getElementById("SSOIPInputText").value = data.SSOIP;
            // document.getElementById("PushNotifyPrefixInputText").value = data.PushNotifyPrefix;
            // if ( data.EnablePushNotify == "1" )
            //     document.getElementById('EnablePushNotifyCheckBtn').click();

            document.getElementById("systemconfigupdatebtn").disabled = false;
        }

        if ( document.getElementById("algorithmconfigformid") != null )
        {
            //console.log("document.getElementById(algorithmconfigformid) != null");

            // update to global
            glSystemConfig["ValidDataTime"] = data.ValidDataTime;
            glSystemConfig["MaxRangindDistance"] = data.MaxRangindDistance;
            glSystemConfig["EnableNoiseMakup"] = data.EnableNoiseMakup;
            glSystemConfig["NoiseMakupRatio"] = data.NoiseMakupRatio;
            glSystemConfig["NoiseMakupWeight"] = data.NoiseMakupWeight;
            glSystemConfig["EnableKalman1D"] = data.EnableKalman1D;
            glSystemConfig["EnableRangingReliability"] = data.EnableRangingReliability;
            glSystemConfig["RangingReliabilityMaxDistance"] = data.RangingReliabilityMaxDistance;
            glSystemConfig["RangingReliabilityMinCount"] = data.RangingReliabilityMinCount;
            glSystemConfig["RangingReliabilityAverageDiff"] = data.RangingReliabilityAverageDiff;

            glSystemConfig["EnableAnchor1DMapping"] = data.EnableAnchor1DMapping;
            glSystemConfig["EnableOneAnchorCalculation"] = data.EnableOneAnchorCalculation;
            glSystemConfig["EnableGDOP"] = data.EnableGDOP;
            glSystemConfig["GDOPThreshold"] = data.GDOPThreshold;
            glSystemConfig["CalculationMethod"] = data.CalculationMethod;
            glSystemConfig["EnableIMUVelocity"] = data.EnableIMUVelocity;
            glSystemConfig["IMUVelocityOffset"] = data.IMUVelocityOffset;
            glSystemConfig["SteadyStateOffset"] = data.SteadyStateOffset;
            glSystemConfig["WeightMovingAverageValue1"] = data.WeightMovingAverageValue1;
            glSystemConfig["WeightMovingAverageValue2"] = data.WeightMovingAverageValue2;
            glSystemConfig["WeightMovingAverageValue3"] = data.WeightMovingAverageValue3;
            glSystemConfig["StayInWallTimeout"] = data.StayInWallTimeout;
            glSystemConfig["EnableFixHeight"] = data.EnableFixHeight;

            glSystemConfig["SmoothingResult"] = data.SmoothingResult;
            glSystemConfig["SmoothingResultOffset"] = data.SmoothingResultOffset;
            glSystemConfig["SmoothingResultStep"] = data.SmoothingResultStep;
            glSystemConfig["SmoothingResultFirstStateOffsetRatio"] = data.SmoothingResultFirstStateOffsetRatio;
            glSystemConfig["SmoothingResultFirstStateRunTime"] = data.SmoothingResultFirstStateRunTime;
            glSystemConfig["SmoothingResultSecondStateOffsetRatio"] = data.SmoothingResultSecondStateOffsetRatio;
            glSystemConfig["SmoothingResultSecondStateRunTime"] = data.SmoothingResultSecondStateRunTime;

            document.getElementById("ValidDataTimeInputText").value = data.ValidDataTime;
            document.getElementById("MaxRangingDistanceInputText").value = data.MaxRangindDistance;
            if ( data.EnableNoiseMakup == "1" )
                document.getElementById('NoiseMakupCheckBtn').click();
            document.getElementById("NoiseMakupRatioInputText").value = data.NoiseMakupRatio;
            document.getElementById("NoiseMakupWeightInputText").value = data.NoiseMakupWeight;
            if ( data.EnableKalman1D == "1" )
                document.getElementById('EnableKalman1DCheckBtn').click();
            if ( data.EnableRangingReliability == "1" )
                document.getElementById('EnableRangingReliabilityCheckBtn').click();

            document.getElementById("RangingReliabilityMaxDistanceInputText").value = data.RangingReliabilityMaxDistance;
            document.getElementById("RangingReliabilityMinCountInputText").value = data.RangingReliabilityMinCount;
            document.getElementById("RangingReliabilityAverageDiffInputText").value = data.RangingReliabilityAverageDiff;

            if ( data.EnableAnchor1DMapping == "1" )
                document.getElementById('Anchor1DMappingCheckBtn').click();
            if ( data.EnableOneAnchorCalculation == "1" )
                document.getElementById('OneAnchorCalculationCheckBtn').click();
            if ( data.EnableGDOP == "1" )
                document.getElementById('GDOPresolveCheckBtn').click();
            document.getElementById("GDOPThresholdInputText").value = data.GDOPThreshold;
            if ( data.EnableIMUVelocity == "1" )
                document.getElementById('IMUVelocityCheckBtn').click();
            document.getElementById("IMUVelocityOffsetInputText").value = data.IMUVelocityOffset;
            document.getElementById("SteadyStateOffsetInputText").value = data.SteadyStateOffset;
            document.getElementById("StayInWallTimeoutInputText").value = data.StayInWallTimeout;
            if ( data.EnableFixHeight == "1" )
                document.getElementById('EnableFixHeightCheckBtn').click();

            if ( data.CalculationMethod == "0" )
                document.getElementById('optionsPositionAlgorithm1').click();
            if ( data.CalculationMethod == "1" )
                document.getElementById('optionsPositionAlgorithm2').click();
            if ( data.CalculationMethod == "2" )
                document.getElementById('optionsPositionAlgorithm3').click();
            document.getElementById("WeightMovingAverageInputText1").value = data.WeightMovingAverageValue1;
            document.getElementById("WeightMovingAverageInputText2").value = data.WeightMovingAverageValue2;
            document.getElementById("WeightMovingAverageInputText3").value = data.WeightMovingAverageValue3;

            if ( data.SmoothingResult == "1" )
                document.getElementById('SmoothingResultCheckBtn').click();

            document.getElementById("SmoothingResultOffsetInputText").value = data.SmoothingResultOffset;
            document.getElementById("SmoothingResultStepInputText").value = data.SmoothingResultStep;
            document.getElementById("SmoothingResultFirstStateOffsetRatioInputText").value = data.SmoothingResultFirstStateOffsetRatio;
            document.getElementById("SmoothingResultFirstStateRunTimeInputText").value = data.SmoothingResultFirstStateRunTime;
            document.getElementById("SmoothingResultSecondStateOffsetRatioInputText").value = data.SmoothingResultSecondStateOffsetRatio;
            document.getElementById("SmoothingResultSecondStateRunTimeInputText").value = data.SmoothingResultSecondStateRunTime;

            document.getElementById("algorithmconfigupdatebtn").disabled = false;
        }

        if ( document.getElementById("lorawanconfigformid") != null )
        {
            //console.log("document.getElementById(lorawanconfigformid) != null");

            document.getElementById("LoRAWANTPOICInputText").value = data.LoRAWANTPOIC;
            document.getElementById("MQTTHostIpInputText").value = data.MQTTHostIp;
            document.getElementById("MQTTHostPortInputText").value = data.MQTTHostPort;
            document.getElementById("MQTTHostUsernameInputText").value = data.MQTTHostUsername;
            document.getElementById("MQTTHostPasswordInputText").value = data.MQTTHostPassword;
            document.getElementById("MQTTClientIdInputText").value = data.MQTTClientId;

            document.getElementById("lorawanconfigupdatebtn").disabled = false;
        }

    });
}

function load_system_status()
{
    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemstatus=1'+'&project_id='+project_id, function(data)
    {
        if ( document.getElementById("systemstatusarea") != null )
        {
            var StatusString = "";
            var status_num = parseInt(data['STATUS_NUM']);
            for (var i = 1; i <= status_num; ++i)
            {
                var key = 'STATUS_'+i;
                var status = data[key];
                StatusString += status;
                StatusString += '\n';
            }

            document.getElementById("systemstatusarea").value = StatusString;
        }
    });
}

$("#algorithmconfigupdatebtn").click(function()
{
    var ValidDataTime = document.getElementById("ValidDataTimeInputText").value;
    var MaxRangindDistance = document.getElementById("MaxRangingDistanceInputText").value;
    var EnableNoiseMakup = 0;
    if ( document.getElementById('NoiseMakupCheckBtn').checked )
        EnableNoiseMakup = 1;
    var NoiseMakupRatio = document.getElementById("NoiseMakupRatioInputText").value;
    var NoiseMakupWeight = document.getElementById("NoiseMakupWeightInputText").value;
    var EnableKalman1D = 0;
    if ( document.getElementById('EnableKalman1DCheckBtn').checked )
        EnableKalman1D = 1;
    var EnableRangingReliability = 0;
    if ( document.getElementById('EnableRangingReliabilityCheckBtn').checked )
        EnableRangingReliability = 1;
    var RangingReliabilityMaxDistance = document.getElementById("RangingReliabilityMaxDistanceInputText").value;
    var RangingReliabilityMinCount = document.getElementById("RangingReliabilityMinCountInputText").value;
    var RangingReliabilityAverageDiff = document.getElementById("RangingReliabilityAverageDiffInputText").value;

    var EnableAnchor1DMapping = 0;
    if ( document.getElementById('Anchor1DMappingCheckBtn').checked )
        EnableAnchor1DMapping = 1;
    var EnableOneAnchorCalculation = 0;
    if ( document.getElementById('OneAnchorCalculationCheckBtn').checked )
        EnableOneAnchorCalculation = 1;
    var EnableGDOP = 0;
    if ( document.getElementById('GDOPresolveCheckBtn').checked )
        EnableGDOP = 1;
    var GDOPThreshold = document.getElementById("GDOPThresholdInputText").value;
    var EnableIMUVelocity = 0;
    if ( document.getElementById('IMUVelocityCheckBtn').checked )
        EnableIMUVelocity = 1;
    var IMUVelocityOffset = document.getElementById("IMUVelocityOffsetInputText").value;
    var SteadyStateOffset = document.getElementById("SteadyStateOffsetInputText").value;
    var StayInWallTimeout = document.getElementById("StayInWallTimeoutInputText").value;

    var EnableFixHeight = 0;
    if ( document.getElementById('EnableFixHeightCheckBtn').checked )
        EnableFixHeight = 1;

    var CalculationMethod = 0;
    if ( document.getElementById('optionsPositionAlgorithm1').checked )
        CalculationMethod = 0;
    if ( document.getElementById('optionsPositionAlgorithm2').checked )
        CalculationMethod = 1;
    if ( document.getElementById('optionsPositionAlgorithm3').checked )
        CalculationMethod = 2;
    var WeightMovingAverageValue1 = document.getElementById("WeightMovingAverageInputText1").value;
    var WeightMovingAverageValue2 = document.getElementById("WeightMovingAverageInputText2").value;
    var WeightMovingAverageValue3 = document.getElementById("WeightMovingAverageInputText3").value;

    var SmoothingResult = 0;
    if ( document.getElementById('SmoothingResultCheckBtn').checked )
        SmoothingResult = 1;
    var SmoothingResultOffset = document.getElementById("SmoothingResultOffsetInputText").value;
    var SmoothingResultStep =  document.getElementById("SmoothingResultStepInputText").value;
    var SmoothingResultFirstStateOffsetRatio =  document.getElementById("SmoothingResultFirstStateOffsetRatioInputText").value;
    var SmoothingResultFirstStateRunTime =  document.getElementById("SmoothingResultFirstStateRunTimeInputText").value;
    var SmoothingResultSecondStateOffsetRatio =  document.getElementById("SmoothingResultSecondStateOffsetRatioInputText").value;
    var SmoothingResultSecondStateRunTime =  document.getElementById("SmoothingResultSecondStateRunTimeInputText").value;

    document.getElementById("algorithmconfigupdatebtn").disabled = true;

    var SendDate = "";
    SendDate += "updateAlgorithmConfig=1";
    if (ValidDataTime != glSystemConfig["ValidDataTime"])
        SendDate += "&ValidDataTime=" + ValidDataTime;
    if (MaxRangindDistance != glSystemConfig["MaxRangindDistance"])
        SendDate += "&MaxRangindDistance=" + MaxRangindDistance;
    if (EnableNoiseMakup != glSystemConfig["EnableNoiseMakup"])
        SendDate += "&EnableNoiseMakup=" + EnableNoiseMakup;
    if (NoiseMakupRatio != glSystemConfig["NoiseMakupRatio"])
        SendDate += "&NoiseMakupRatio=" + NoiseMakupRatio;
    if (NoiseMakupWeight != glSystemConfig["NoiseMakupWeight"])
        SendDate += "&NoiseMakupWeight=" + NoiseMakupWeight;
    if (EnableKalman1D != glSystemConfig["EnableKalman1D"])
        SendDate += "&EnableKalman1D=" + EnableKalman1D;
    if (EnableRangingReliability != glSystemConfig["EnableRangingReliability"])
        SendDate += "&EnableRangingReliability=" + EnableRangingReliability;
    if (RangingReliabilityMaxDistance != glSystemConfig["RangingReliabilityMaxDistance"])
        SendDate += "&RangingReliabilityMaxDistance=" + RangingReliabilityMaxDistance;
    if (RangingReliabilityMinCount != glSystemConfig["RangingReliabilityMinCount"])
        SendDate += "&RangingReliabilityMinCount=" + RangingReliabilityMinCount;
    if (RangingReliabilityAverageDiff != glSystemConfig["RangingReliabilityAverageDiff"])
        SendDate += "&RangingReliabilityAverageDiff=" + RangingReliabilityAverageDiff;

    if (EnableAnchor1DMapping != glSystemConfig["EnableAnchor1DMapping"])
        SendDate += "&EnableAnchor1DMapping=" + EnableAnchor1DMapping;
    if (EnableOneAnchorCalculation != glSystemConfig["EnableOneAnchorCalculation"])
        SendDate += "&EnableOneAnchorCalculation=" + EnableOneAnchorCalculation;
    if (EnableGDOP != glSystemConfig["EnableGDOP"])
        SendDate += "&EnableGDOP=" + EnableGDOP;
    if (GDOPThreshold != glSystemConfig["GDOPThreshold"])
        SendDate += "&GDOPThreshold=" + GDOPThreshold;
    if (CalculationMethod != glSystemConfig["CalculationMethod"])
        SendDate += "&CalculationMethod=" + CalculationMethod;
    if (EnableIMUVelocity != glSystemConfig["EnableIMUVelocity"])
        SendDate += "&EnableIMUVelocity=" + EnableIMUVelocity;
    if (IMUVelocityOffset != glSystemConfig["IMUVelocityOffset"])
        SendDate += "&IMUVelocityOffset=" + IMUVelocityOffset;
    if (SteadyStateOffset != glSystemConfig["SteadyStateOffset"])
        SendDate += "&SteadyStateOffset=" + SteadyStateOffset;
    if (WeightMovingAverageValue1 != glSystemConfig["WeightMovingAverageValue1"])
        SendDate += "&WeightMovingAverageValue1=" + WeightMovingAverageValue1;
    if (WeightMovingAverageValue2 != glSystemConfig["WeightMovingAverageValue2"])
        SendDate += "&WeightMovingAverageValue2=" + WeightMovingAverageValue2;
    if (WeightMovingAverageValue3 != glSystemConfig["WeightMovingAverageValue3"])
        SendDate += "&WeightMovingAverageValue3=" + WeightMovingAverageValue3;
    if (StayInWallTimeout != glSystemConfig["StayInWallTimeout"])
        SendDate += "&StayInWallTimeout=" + StayInWallTimeout;
    if (EnableFixHeight != glSystemConfig["EnableFixHeight"])
        SendDate += "&EnableFixHeight=" + EnableFixHeight;

    if (SmoothingResult != glSystemConfig["SmoothingResult"])
        SendDate += "&SmoothingResult=" + SmoothingResult;
    if (SmoothingResultOffset != glSystemConfig["SmoothingResultOffset"])
        SendDate += "&SmoothingResultOffset=" + SmoothingResultOffset;
    if (SmoothingResultStep != glSystemConfig["SmoothingResultStep"])
        SendDate += "&SmoothingResultStep=" + SmoothingResultStep;
    if (SmoothingResultFirstStateOffsetRatio != glSystemConfig["SmoothingResultFirstStateOffsetRatio"])
        SendDate += "&SmoothingResultFirstStateOffsetRatio=" + SmoothingResultFirstStateOffsetRatio;
    if (SmoothingResultFirstStateRunTime != glSystemConfig["SmoothingResultFirstStateRunTime"])
        SendDate += "&SmoothingResultFirstStateRunTime=" + SmoothingResultFirstStateRunTime;
    if (SmoothingResultSecondStateOffsetRatio != glSystemConfig["SmoothingResultSecondStateOffsetRatio"])
        SendDate += "&SmoothingResultSecondStateOffsetRatio=" + SmoothingResultSecondStateOffsetRatio;
    if (SmoothingResultSecondStateRunTime != glSystemConfig["SmoothingResultSecondStateRunTime"])
        SendDate += "&SmoothingResultSecondStateRunTime=" + SmoothingResultSecondStateRunTime;

    SendDate += '&project_id='+project_id;

    var targetURL = gl_target_server + "/php/configvils.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            glSystemConfig["ValidDataTime"] = ValidDataTime;
            glSystemConfig["MaxRangindDistance"] = MaxRangindDistance;
            glSystemConfig["EnableNoiseMakup"] = EnableNoiseMakup;
            glSystemConfig["NoiseMakupRatio"] = NoiseMakupRatio;
            glSystemConfig["NoiseMakupWeight"] = NoiseMakupWeight;
            glSystemConfig["EnableKalman1D"] = EnableKalman1D;
            glSystemConfig["EnableRangingReliability"] = EnableRangingReliability;
            glSystemConfig["RangingReliabilityMaxDistance"] = RangingReliabilityMaxDistance;
            glSystemConfig["RangingReliabilityMinCount"] = RangingReliabilityMinCount;
            glSystemConfig["RangingReliabilityAverageDiff"] = RangingReliabilityAverageDiff;
            glSystemConfig["EnableGDOP"] = EnableGDOP;
            glSystemConfig["GDOPThreshold"] = GDOPThreshold;
            glSystemConfig["CalculationMethod"] = CalculationMethod;
            glSystemConfig["EnableIMUVelocity"] = EnableIMUVelocity;
            glSystemConfig["IMUVelocityOffset"] = IMUVelocityOffset;
            glSystemConfig["SteadyStateOffset"] = SteadyStateOffset;
            glSystemConfig["WeightMovingAverageValue1"] = WeightMovingAverageValue1;
            glSystemConfig["WeightMovingAverageValue2"] = WeightMovingAverageValue2;
            glSystemConfig["WeightMovingAverageValue3"] = WeightMovingAverageValue3;
            glSystemConfig["StayInWallTimeout"] = StayInWallTimeout;
            glSystemConfig["EnableFixHeight"] = EnableFixHeight;
            glSystemConfig["SmoothingResult"] = SmoothingResult;
            glSystemConfig["SmoothingResultOffset"] = SmoothingResultOffset;
            glSystemConfig["SmoothingResultStep"] = SmoothingResultStep;
            glSystemConfig["SmoothingResultFirstStateOffsetRatio"] = SmoothingResultFirstStateOffsetRatio;
            glSystemConfig["SmoothingResultFirstStateRunTime"] = SmoothingResultFirstStateRunTime;
            glSystemConfig["SmoothingResultSecondStateOffsetRatio"] = SmoothingResultSecondStateOffsetRatio;
            glSystemConfig["SmoothingResultSecondStateRunTime"] = SmoothingResultSecondStateRunTime;
        },
      error:
        function(xhr, status, error)
        {
            //document.getElementById("syncnode_success").innerText = "";
            //document.getElementById("syncnode_failed").innerText = xhr.responseText;
        },
      complete:
        function()
        {

            check_algorithmconfig_update();

        }
    });

    return false;
});

function check_algorithmconfig_update()
{
    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {
        var difference = 0;
        difference += (glSystemConfig["ValidDataTime"] != data.ValidDataTime);
        difference += (glSystemConfig["MaxRangindDistance"] != data.MaxRangindDistance);
        difference += (glSystemConfig["EnableNoiseMakup"] != data.EnableNoiseMakup);
        difference += (glSystemConfig["NoiseMakupRatio"] != data.NoiseMakupRatio);
        difference += (glSystemConfig["NoiseMakupWeight"] != data.NoiseMakupWeight);
        difference += (glSystemConfig["EnableKalman1D"] != data.EnableKalman1D);
        difference += (glSystemConfig["EnableRangingReliability"] != data.EnableRangingReliability);
        difference += (glSystemConfig["RangingReliabilityMaxDistance"] != data.RangingReliabilityMaxDistance);
        difference += (glSystemConfig["RangingReliabilityMinCount"] != data.RangingReliabilityMinCount);
        difference += (glSystemConfig["RangingReliabilityAverageDiff"] != data.RangingReliabilityAverageDiff);

        difference += (glSystemConfig["EnableAnchor1DMapping"] != data.EnableAnchor1DMapping);
        difference += (glSystemConfig["EnableOneAnchorCalculation"] != data.EnableOneAnchorCalculation);
        difference += (glSystemConfig["EnableGDOP"] != data.EnableGDOP);
        difference += (glSystemConfig["GDOPThreshold"] != data.GDOPThreshold);
        difference += (glSystemConfig["CalculationMethod"] != data.CalculationMethod);
        difference += (glSystemConfig["EnableIMUVelocity"] != data.EnableIMUVelocity);
        difference += (glSystemConfig["IMUVelocityOffset"] != data.IMUVelocityOffset);
        difference += (glSystemConfig["SteadyStateOffset"] != data.SteadyStateOffset);
        difference += (glSystemConfig["WeightMovingAverageValue1"] != data.WeightMovingAverageValue1);
        difference += (glSystemConfig["WeightMovingAverageValue2"] != data.WeightMovingAverageValue2);
        difference += (glSystemConfig["WeightMovingAverageValue3"] != data.WeightMovingAverageValue3);
        difference += (glSystemConfig["StayInWallTimeout"] != data.StayInWallTimeout);
        difference += (glSystemConfig["EnableFixHeight"] != data.EnableFixHeight);

        difference += (glSystemConfig["SmoothingResult"] != data.SmoothingResult);
        difference += (glSystemConfig["SmoothingResultOffset"] != data.SmoothingResultOffset);
        difference += (glSystemConfig["SmoothingResultStep"] != data.SmoothingResultStep);
        difference += (glSystemConfig["SmoothingResultFirstStateOffsetRatio"] != data.SmoothingResultFirstStateOffsetRatio);
        difference += (glSystemConfig["SmoothingResultFirstStateRunTime"] != data.SmoothingResultFirstStateRunTime);
        difference += (glSystemConfig["SmoothingResultSecondStateOffsetRatio"] != data.SmoothingResultSecondStateOffsetRatio);
        difference += (glSystemConfig["SmoothingResultSecondStateRunTime"] != data.SmoothingResultSecondStateRunTime);


        console.log("check_algorithmconfig_update() difference:"+difference);

        if ( difference > 0 )
        {
            document.getElementById("algorithmconfig_warning_callout").style.display = 'block';
            setTimeout(check_algorithmconfig_update, 1000);
        }
        else
        {
            console.log("check_algorithmconfig_update() update successfully!");

            document.getElementById("algorithmconfig_warning_callout").style.display = 'none';
            document.getElementById("algorithmconfigupdate_callout").style.display = 'block';
            setTimeout(close_callout, 2500, "algorithmconfigupdate_callout");

            document.getElementById("algorithmconfigupdatebtn").disabled = false;
        }

    });
}

$("#systemconfigupdatebtn").click(function()
{
    var CenterWebPort = document.getElementById('CenterWebPortInputText').value;
    var CenterUdpPort = document.getElementById('CenterUdpPortInputText').value;
    var CoordUdpPort = document.getElementById('CoordUdpPortInputText').value;
    var center_broadcast_port = document.getElementById('CenterBroadcastPortInputText').value;
    var WebSocketPort = document.getElementById('CenterWebSocketPortInputText').value;
    var TagAlertTimeout = document.getElementById('TagAlertTimeoutInputText').value;
    var TagVoltageLow = document.getElementById('TagVoltageLowInputText').value;
    var VILSServer = document.getElementById('VILSServerInputText').value;
    var ERPServer = document.getElementById('ERPServerInputText').value;
    var SSOIP = document.getElementById('SSOIPInputText').value;
    // var PushNotifyPrefix = document.getElementById('PushNotifyPrefixInputText').value;

    // var EnablePushNotify = 0;
    // if ( document.getElementById('EnablePushNotifyCheckBtn').checked )
    //     EnablePushNotify = 1;

    document.getElementById("systemconfigupdatebtn").disabled = true;

    var SendDate = "";
    SendDate += "setcenterwebport=" + CenterWebPort;
    SendDate += "&";
    SendDate += "setcenterudpport=" + CenterUdpPort;
    SendDate += "&";
    SendDate += "setcoordudpport=" + CoordUdpPort;
    SendDate += "&";
    SendDate += "setcenterbroadcastport=" + center_broadcast_port;
    SendDate += "&";
    SendDate += "setwebsocketport=" + WebSocketPort;
    SendDate += "&";
    SendDate += "settagalerttimeout=" + TagAlertTimeout;
    SendDate += "&";
    SendDate += "settagvoltagelow=" + TagVoltageLow;
    SendDate += "&";
    SendDate += "setvilsserver=" + VILSServer;
    SendDate += "&";
    SendDate += "seterpserver=" + ERPServer;
    SendDate += "&";
    SendDate += "setssoip=" + SSOIP;
    // SendDate += "&";
    // SendDate += "setpushnotifyprefix=" + PushNotifyPrefix;
    // SendDate += "&";
    // SendDate += "setenablepushnotify=" + EnablePushNotify;
    SendDate += "&";
    SendDate += 'project_id='+project_id;

    var targetURL = gl_target_server + "/php/configvils.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
        },
      error:
        function(xhr, status, error)
        {
            //document.getElementById("syncnode_success").innerText = "";
            //document.getElementById("syncnode_failed").innerText = xhr.responseText;
        },
      complete:
        function()
        {

            check_systemconfig_update();
        }
    });

    return false;
});

function check_systemconfig_update()
{
    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {
        var difference = 0;

        difference += (document.getElementById("CenterWebPortInputText").value != data.web_udp_port);
        difference += (document.getElementById("CenterUdpPortInputText").value != data.center_udp_port);
        difference += (document.getElementById("CoordUdpPortInputText").value != data.coord_udp_port);
        difference += (document.getElementById("CenterBroadcastPortInputText").value != data.center_broadcast_port);
        difference += (document.getElementById("CenterWebSocketPortInputText").value != data.web_socket_port);
        difference += (document.getElementById("TagAlertTimeoutInputText").value != data.TagAlertTimeout);
        difference += (document.getElementById("TagVoltageLowInputText").value != data.TagAlertVoltageLow);
        difference += (document.getElementById("VILSServerInputText").value != data.VILSServer);
        difference += (document.getElementById("ERPServerInputText").value != data.ERPServer);
        difference += (document.getElementById("SSOIPInputText").value != data.SSOIP);
        // difference += (document.getElementById("PushNotifyPrefixInputText").value != data.PushNotifyPrefix);
        // if ( data.EnablePushNotify == "1" && !document.getElementById('EnablePushNotifyCheckBtn').checked )
        //     difference += 1;
        // if ( data.EnablePushNotify == "0" && document.getElementById('EnablePushNotifyCheckBtn').checked )
        //     difference += 1;

        console.log("check_systemconfig_update() difference:"+difference);

        if ( difference > 0 )
        {
            document.getElementById("systemconfig_warning_callout").style.display = 'block';
            setTimeout(check_systemconfig_update, 1000);
        }
        else
        {
            console.log("check_systemconfig_update() update successfully!");

            document.getElementById("systemconfig_warning_callout").style.display = 'none';
            document.getElementById("systemconfigupdate_callout").style.display = 'block';
            setTimeout(close_callout, 2500, "systemconfigupdate_callout");

            document.getElementById("systemconfigupdatebtn").disabled = false;
        }

    });
}

$("#lorawanconfigupdatebtn").click(function()
{
    var LoRAWANTPOIC = document.getElementById('LoRAWANTPOICInputText').value;
    var MQTTHostIp = document.getElementById('MQTTHostIpInputText').value;
    var MQTTHostPort = document.getElementById('MQTTHostPortInputText').value;
    var MQTTHostUsername = document.getElementById('MQTTHostUsernameInputText').value;
    var MQTTHostPassword = document.getElementById('MQTTHostPasswordInputText').value;
    var MQTTClientId = document.getElementById('MQTTClientIdInputText').value;

    document.getElementById("lorawanconfigupdatebtn").disabled = true;

    var SendDate = "";
    SendDate += "updateLoRAWANConfig=1";
    SendDate += "&";
    SendDate += "LoRAWANTPOIC=" + LoRAWANTPOIC;
    SendDate += "&";
    SendDate += "MQTTHostIp=" + MQTTHostIp;
    SendDate += "&";
    SendDate += "MQTTHostPort=" + MQTTHostPort;
    SendDate += "&";
    SendDate += "MQTTHostUsername=" + MQTTHostUsername;
    SendDate += "&";
    SendDate += "MQTTHostPassword=" + MQTTHostPassword;
    SendDate += "&";
    SendDate += "MQTTClientId=" + MQTTClientId;
    SendDate += '&project_id='+project_id;

    var targetURL = gl_target_server + "/php/configvils.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
        },
      error:
        function(xhr, status, error)
        {
            //document.getElementById("syncnode_success").innerText = "";
            //document.getElementById("syncnode_failed").innerText = xhr.responseText;
        },
      complete:
        function()
        {

            check_lorawanconfig_update();
        }
    });

    return false;
});

$("#restlorabtn").click(function()
{
    var SendDate = "resetLoRA=1";

    var targetURL = gl_target_server + "/php/configvils.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
        },
      error:
        function(xhr, status, error)
        {
            //document.getElementById("syncnode_success").innerText = "";
            //document.getElementById("syncnode_failed").innerText = xhr.responseText;
        },
      complete:
        function()
        {

            check_lorawanconfig_update();
        }
    });

    return false;
});
function check_lorawanconfig_update()
{
    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {
        var difference = 0;

        difference += (document.getElementById("LoRAWANTPOICInputText").value != data.LoRAWANTPOIC);
        difference += (document.getElementById("MQTTHostIpInputText").value != data.MQTTHostIp);
        difference += (document.getElementById("MQTTHostPortInputText").value != data.MQTTHostPort);
        difference += (document.getElementById("MQTTHostUsernameInputText").value != data.MQTTHostUsername);
        difference += (document.getElementById("MQTTHostPasswordInputText").value != data.MQTTHostPassword);
        difference += (document.getElementById("MQTTClientIdInputText").value != data.MQTTClientId);

        console.log("check_lorawanconfig_update() difference:"+difference);

        if ( difference > 0 )
        {
            document.getElementById("lorawanconfig_warning_callout").style.display = 'block';
            setTimeout(check_lorawanconfig_update, 1000);
        }
        else
        {
            console.log("check_lorawanconfig_update() update successfully!");

            document.getElementById("lorawanconfig_warning_callout").style.display = 'none';
            document.getElementById("lorawanconfigupdate_callout").style.display = 'block';
            setTimeout(close_callout, 2500, "lorawanconfigupdate_callout");

            document.getElementById("lorawanconfigupdatebtn").disabled = false;
        }

    });
}

$("#prjsavebtn").click(function()
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['PROJECT_NAME'] = document.getElementById("Local_Project_Name").value;
    jsondata['PROJECT_DESCRIPTION'] = document.getElementById("Local_Project_Desc").value;
    // jsondata['WGS48OriginX'] = document.getElementById("WGS48OriginX").value;
    // jsondata['WGS48OriginY'] = document.getElementById("WGS48OriginY").value;
    // jsondata['MAP_North'] = document.getElementById("MAP_North").value;
    // jsondata['BuildingOriginX'] = document.getElementById("BuildingOriginX").value;
    // jsondata['BuildingOriginY'] = document.getElementById("BuildingOriginY").value;
    // jsondata['BuildingXLength'] = document.getElementById("BuildingXLength").value;
    // jsondata['BuildingYLength'] = document.getElementById("BuildingYLength").value;
    // jsondata['BuildingID'] = document.getElementById("BuildingID").value;
    // jsondata['BuildingFloor'] = document.getElementById("BuildingFloor").value;

    // disable button
    document.getElementById("prjsavebtn").innerText = "updating...";
    document.getElementById("prjsavebtn").disabled = true;
    var targetURL = gl_target_server + "/php/configvils.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
            // alert("update success!");

            check_vilssetting_update();

            // enable button
            // document.getElementById("prjsavebtn").disabled = false;
            // document.getElementById("prjsavebtn").innerText = "Save";
            //
            // window.location = "/index.php/vilssettings/"+project_id;
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
            // enable button
            document.getElementById("prjsavebtn").disabled = false;
            document.getElementById("prjsavebtn").innerText = "Save";
        }
   });
   return false;
});


function check_vilssetting_update()
{
    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadconfig=1'+'&project_id='+project_id , function(data)
    {
        var difference = 0;

        difference += (document.getElementById("Local_Project_Name").value != data.PROJECT_NAME);
        difference += (document.getElementById("Local_Project_Desc").value != data.PROJECT_DESCRIPTION);
        // difference += (document.getElementById("WGS48OriginX").value != data.WGS48OriginX);
        // difference += (document.getElementById("WGS48OriginY").value != data.WGS48OriginY);
        // difference += (document.getElementById("MAP_North").value != data.MAP_North);
        // difference += (document.getElementById("BuildingOriginX").value != data.BuildingOriginX);
        // difference += (document.getElementById("BuildingOriginY").value != data.BuildingOriginY);
        // difference += (document.getElementById("BuildingXLength").value != data.BuildingXLength);
        // difference += (document.getElementById("BuildingYLength").value != data.BuildingYLength);
        // difference += (document.getElementById("BuildingID").value != data.BuildingID);
        // difference += ( document.getElementById("BuildingFloor").value != data.BuildingFloor);

        console.log("check_vilssetting_update() difference:"+difference);

        if ( difference > 0 )
        {
            document.getElementById("vilssetting_warning_callout").style.display = 'block';
            setTimeout(check_vilssetting_update, 1000);
        }
        else
        {
            console.log("check_vilssetting_update() update successfully!");

            document.getElementById("vilssetting_warning_callout").style.display = 'none';
            document.getElementById("vilssettingupdate_callout").style.display = 'block';
            setTimeout(reload_settingpage, 500);
        }

    });
}

function close_callout(ui_id)
{
    document.getElementById(ui_id).style.display = 'none';
}

function reload_settingpage()
{
    document.getElementById("prjsavebtn").disabled = false;
    document.getElementById("prjsavebtn").innerText = "Save";

    window.location = "/index.php/vilssettings/"+project_id;
}

$("#restartappbtn").click(function()
{
    var retVal = confirm("重新啟動系統?");
    if( retVal == true )
    {
        var jsondata = {};
        jsondata['project_id'] = ''+project_id;
        jsondata['restart_app'] = 1;
        var targetURL = gl_target_server + "/php/configvils.php";

        jQuery.ajax({
          url: targetURL,
          type: 'POST',
          data: jsondata,
          success:
            function(response)
            {
                 //alert("response="+response);
                 //var config = JSON.parse(response);

                 //window.location = "/index.php/"+project_id;
                 //window.location = "/index.php/vilssettings/"+project_id;
                 //document.getElementById('contentIframe').src = "/index.php/vilssettings/"+project_id;
                 window.location = "/index.php/vilssettings/"+project_id;
            },
          error:
            function(xhr, status, error)
            {
                alert("error = " + error);
                alert("xhr.responseText = " + xhr.responseText);
            }
       });
    }
    return false;
});

$("#deletethisprojectbtn").click(function()
{
    var alertString = "This step can not undo, Are you should want to delete this project?";
    console.log("alertString="+alertString);
    var r = window.confirm(alertString);
    if (r == true)
    {
        //var jsondata = {};
        //jsondata['project_id'] = ''+project_id;
        //jsondata['delete_project'] = 1;

        //var paramdata = '?' + $.param({"delete_project": 1, "project_id" : project_id});
        //var targetURL = gl_target_server + "/php/configvils.php";

        var SendData = "project_id="+project_id;
        SendData += "&delete_project=1";

        jQuery.ajax({
         url: '/php/configvils.php',
         type: 'POST',
         data: SendData,
         dataType: 'text',
        // jQuery.ajax({
        //   url: targetURL + paramdata,
        //   type: 'DELETE',
        //   data: jsondata,
          success:
            function(response)
            {
                 console.log("response="+response);
                 var config = JSON.parse(response);

                 if ( parent.$('#contentIframe')[0] != undefined )
                 {
                     // inside frame
                     parent.window.location = "/index.php/"+parseInt(config.PROJECT_ID);
                 }
                 else
                 {
                     window.location = "/index.php/vilssettings/"+parseInt(config.PROJECT_ID);
                 }
            },
          error:
            function(xhr, status, error)
            {
                alert("error = " + error);
                alert("xhr.responseText = " + xhr.responseText);
            }
       });
    }
    return false;
});

function EnableRangingReliabilityCheckBtn_onclick()
{
    if(document.getElementById('EnableRangingReliabilityCheckBtn').checked)
        document.getElementById("RangingReliabilityInputText_ul").style.display = 'block';
    else
        document.getElementById("RangingReliabilityInputText_ul").style.display = 'none';
}

function NoiseMakupCheckBtn_onclick()
{
    if(document.getElementById('NoiseMakupCheckBtn').checked)
        document.getElementById("NoiseMakupCheckBtn_ul").style.display = 'block';
    else
        document.getElementById("NoiseMakupCheckBtn_ul").style.display = 'none';
}

function optionsPositionAlgorithm1_onclick()
{
    document.getElementById("optionsPositionAlgorithm3_ul").style.display = 'none';
}

function optionsPositionAlgorithm2_onclick()
{
    document.getElementById("optionsPositionAlgorithm3_ul").style.display = 'none';
}

function optionsPositionAlgorithm3_onclick()
{
    document.getElementById("optionsPositionAlgorithm3_ul").style.display = 'block';
}

function GDOPresolveCheckBtn_onclick()
{
    if(document.getElementById('GDOPresolveCheckBtn').checked)
        document.getElementById("GDOPresolveInputText_ul").style.display = 'block';
    else
        document.getElementById("GDOPresolveInputText_ul").style.display = 'none';
}

function IMUVelocityCheckBtn_onclick()
{
    if(document.getElementById('IMUVelocityCheckBtn').checked)
        document.getElementById("IMUVelocityOffsetInputText_ul").style.display = 'block';
    else
        document.getElementById("IMUVelocityOffsetInputText_ul").style.display = 'none';
}

function SmoothingResultCheckBtn_onclick()
{
    if(document.getElementById('SmoothingResultCheckBtn').checked)
        document.getElementById("SmoothingResultOffsetInputText_ul").style.display = 'block';
    else
        document.getElementById("SmoothingResultOffsetInputText_ul").style.display = 'none';
}

function open_settings_page()
{
    var win = window.open('/index.php/vilssettings/'+project_id, '_blank');
    win.focus();
}
