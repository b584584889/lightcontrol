/*
 * Author: ikki Chung
 * Date: 30 Jan 2019
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var glFenceAlertInfo = {};
var glEnableShowFenceAlertMsg = false;

var glDeviceAlertEvent = [];
var SHOW_SAME_ALERT_TIMEOUT = 30;// 30 seconds

check_alert_msg();

function createFenceAlertMsg(tagid, tagname, data_action, data_areaname)
{
    glFenceAlertInfo.data_id = tagid;
    glFenceAlertInfo.data_name = tagname;
    glFenceAlertInfo.data_action = data_action;
    glFenceAlertInfo.data_areaname = data_areaname;
    glEnableShowFenceAlertMsg = true;
}

function show_fence_alert_msg(tagid, tagname, action, areaname)
{

    var currentTime = getCurrentTimeStamp();

    var device_event = glDeviceAlertEvent[tagid];
    if( device_event != undefined )
    {
        if ( device_event.action == action )
        {
            // same action, check timeout
            var diffTime = currentTime - glDeviceAlertEvent[tagid].timestamp;

            console.log("show_fence_alert_msg() action:"+action+", tagid:"+tagid+", tagname:"+tagname +" diffTime:"+diffTime);

            if (diffTime < SHOW_SAME_ALERT_TIMEOUT)
            {
                return;
            }
        }
        else
        {
            // different action
            glDeviceAlertEvent[tagid].action = action;
        }
    }
    else
    {
        glDeviceAlertEvent[tagid] = {};
        glDeviceAlertEvent[tagid].action = action;
        glDeviceAlertEvent[tagid].timestamp = currentTime;
    }
    glDeviceAlertEvent[tagid].timestamp = currentTime;

    var new_HelpVoice_id = 0;
    var showMsg = " alert ";
    if ( parseInt(action) == 1)
    {
        showMsg = "進入電子圍欄 " + areaname;
        // fence_alert_HelpVoice_id_1_alert = 1;
        new_HelpVoice_id = 3;
    }
    else
    if ( parseInt(action) == 2)
    {
        showMsg = "離開電子圍欄 " + areaname;
        // fence_alert_HelpVoice_id_2_alert = 1;
        new_HelpVoice_id = 4;
    }

    //
    // Alert box
    //
    var new_box = create_alert_box(tagid, tagname, showMsg, new_HelpVoice_id);
    if (new_box)
    {

        create_HelpVoice(tagid, new_HelpVoice_id);

        //
        // open map 2d page
        //
        var curURL = parent.document.getElementById('contentIframe').src;
        var n = curURL.indexOf("vils2dmap");
        if (n < 0)
        {
            console.log("curURL:"+ parent.document.getElementById('contentIframe').src);
            parent.show_2dmap(project_id);
        }
    }

    //
    // play sound
    //
    // if (fence_alert_HelpVoice_id_1_alert == 1)
    // {
    //     fence_alert_HelpVoice_id_1 = FenceAlertHelpVoiceAudio1.play();
    //     console.log("show_fence_alert_msg() FenceAlertHelpVoiceAudio1.play() fence_alert_HelpVoice_id_1:"+fence_alert_HelpVoice_id_1);
    // }
    // else
    // if (fence_alert_HelpVoice_id_2_alert == 1)
    // {
    //     fence_alert_HelpVoice_id_2 = FenceAlertHelpVoiceAudio2.play();
    //     console.log("show_fence_alert_msg() FenceAlertHelpVoiceAudio2.play() fence_alert_HelpVoice_id_2:"+fence_alert_HelpVoice_id_2);
    // }

}

function check_alert_msg()
{
    // if (FENCE_ALERT_MANUAL_CLOSE == 0)
    // {
    //     if (glTagFenceAlert_start_timestamp > 0)
    //     {
    //         // auto close
    //         var currentTime = getCurrentTimeStamp();
    //         var diffTime = currentTime - glTagFenceAlert_start_timestamp;
    //         //console.log("check_help_msg() diffTime:"+diffTime +", currentTime:"+currentTime);
    //         var isVisible = $("#tagalertbody").is( ":visible" );
    //
    //         console.log("check_alert_msg() diffTime:"+diffTime +", currentTime:"+currentTime+", isVisible:"+isVisible);
    //
    //         if (!isVisible && (diffTime > 1 && diffTime < 10))
    //         {
    //             //should open alert window
    //             console.log("check_alert_msg() !!!!!!should open alert window!!!!!!!");
    //             $('#tagalertbtn').click();
    //         }
    //
    //         if (isVisible && (diffTime > 10))
    //         {
    //             $('#tagalertclosebtn').click();
    //         }
    //     }
    // }

    if (glEnableShowFenceAlertMsg == true)
    {
        show_fence_alert_msg(glFenceAlertInfo.data_id, glFenceAlertInfo.data_name, glFenceAlertInfo.data_action, glFenceAlertInfo.data_areaname);
        glEnableShowFenceAlertMsg = false;
    }

    setTimeout(check_alert_msg, 200);
}

function stop_fencealert_msg(nodeid, uiclose)
{
    console.log("stop_fencealert_msg("+nodeid+")");

    if( glDeviceAlertEvent[nodeid] != undefined && (uiclose == 0))
    {
        // uiclose == 0  -> 按下Cancel
        // uiclose == 1  -> 網頁關掉視窗
        glDeviceAlertEvent[nodeid] = 0;
    }

    remove_HelpVoice(nodeid);
}

// function stop_fencealert_msg()
// {
//     console.log("stop_fencealert_msg() fence_alert_HelpVoice_id_1_alert:"+fence_alert_HelpVoice_id_1_alert+", fence_alert_HelpVoice_id_2_alert:"+fence_alert_HelpVoice_id_2_alert);
//     glTagFenceAlert_start_timestamp = -1;
//
//     if ( fence_alert_HelpVoice_id_1_alert != 0 )
//     {
//         console.log("stop_fencealert_msg() FenceAlertHelpVoiceAudio1.stop()");
//         //if (fence_alert_HelpVoice_id_1 > 0)
//         FenceAlertHelpVoiceAudio1.stop();
//         fence_alert_HelpVoice_id_1 = -1;
//         fence_alert_HelpVoice_id_1_alert = 0;
//     }
//     if ( fence_alert_HelpVoice_id_2_alert != 0 )
//     {
//         console.log("stop_fencealert_msg() FenceAlertHelpVoiceAudio2.stop()");
//         FenceAlertHelpVoiceAudio2.stop();
//         fence_alert_HelpVoice_id_2 = -1;
//         fence_alert_HelpVoice_id_2_alert = 0;
//     }
// }
