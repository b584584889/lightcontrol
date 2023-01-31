/*
 * Author: ikki Chung
 * Date: 30 Jan 2019
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

// var glHelpVoiceQueue = [];
// var glCurrentPlayHelpVoice_id = -1;

var glDeviceAlertTimestamp = [];
var SHOW_SAME_ALERT_TIMEOUT = 30;// 30 seconds

// var HelpVoiceAudio1 = new Howl({
//   src: ['/audio/人員區域求助.mp3'],
//   autoplay: false,
//   loop: true,
//   html5: true
// });
//
// var HelpVoiceAudio2 = new Howl({
//   src: ['/audio/固定裝置求助.mp3'],
//   autoplay: false,
//   loop: true,
//   html5: true
// });

function show_help_msg(alert, alert0D, tagid, tagname, locatorid, locatorname)
{
    //console.log("show_help_msg() alert:"+alert+", alert0D:"+alert0D+", tagid:"+tagid+", tagname:"+tagname);
    //console.log("show_help_msg() glTagAlert_start_timestamp:"+glTagAlert_start_timestamp +", glTagAlertCloseBtn_click_timestamp:"+glTagAlertCloseBtn_click_timestamp);

    var currentTime = getCurrentTimeStamp();

    var device_TS = glDeviceAlertTimestamp[tagid];
    if( device_TS != undefined )
    {
        var diffTime = currentTime - device_TS;

         //console.log("show_help_msg() alert:"+alert+", alert0D:"+alert0D+", tagid:"+tagid+", tagname:"+tagname +" diffTime:"+diffTime);

        if (diffTime < SHOW_SAME_ALERT_TIMEOUT)
        {
            return;
        }
    }
    glDeviceAlertTimestamp[tagid] = currentTime;

    //console.log("show_help_msg() alert:"+alert+", alert0D:"+alert0D+", tagid:"+tagid+", tagname:"+tagname);

    var new_HelpVoice_id = 0;
    var showMsg = " alert ";
    if ( parseInt(alert) != 0)
    {
        showMsg = "人員區域求助";
        new_HelpVoice_id = 1;
    }
    else
    if ( parseInt(alert0D) != 0)
    {
        if (tagid == locatorid )
        {
            showMsg = "固定裝置求助";
            new_HelpVoice_id = 2;
        }
        else
        if (locatorname.length > 0)
        {
            showMsg = "在固定裝置 "+ locatorname +" 求助";
            new_HelpVoice_id = 2;
        }
        else
        {
            showMsg = "人員區域求助";
            new_HelpVoice_id = 1;
        }
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

}

function stop_help_msg(nodeid, uiclose)
{
    console.log("stop_help_msg("+nodeid+")");

    if( glDeviceAlertTimestamp[nodeid] != undefined && (uiclose == 0))
    {
        // uiclose == 0  -> 按下Cancel
        // uiclose == 1  -> 網頁關掉視窗
        glDeviceAlertTimestamp[nodeid] = 0;
    }

    remove_HelpVoice(nodeid);

    // var index;
    // for(index = 0; index<glHelpVoiceQueue.length;index++)
    // {
    //     var HelpVoiceItem = glHelpVoiceQueue[index];
    //     if ( HelpVoiceItem.nodeid === nodeid )
    //     {
    //         glHelpVoiceQueue.splice(index, 1);
    //         break;
    //     }
    // }
    //
    // play_HelpVoice();

}

function create_CallOut_box(nodeid, nodename)
{
    var currentTime = getCurrentTimeStamp();

    console.log("create_CallOut_box() nodeid:"+nodeid+" nodename:"+nodename);

    var device_TS = glDeviceAlertTimestamp[nodeid];
    if( device_TS != undefined )
    {
        var diffTime = currentTime - device_TS;

        if (diffTime < SHOW_SAME_ALERT_TIMEOUT)
        {
            return;
        }
    }
    glDeviceAlertTimestamp[nodeid] = currentTime;

    var showMsg = "按下呼喚鈕";

    console.log("create_CallOut_box() showMsg:"+showMsg);

    create_alert_box(nodeid, nodename, showMsg, 5);
}

// function create_HelpVoice(tagid, new_HelpVoice_id)
// {
//     var HelpVoiceItem = {};
//     HelpVoiceItem.nodeid = tagid;
//     HelpVoiceItem.HelpVoice_id = new_HelpVoice_id;
//
//     console.log("create_HelpVoice() tagid:"+tagid+" glCurrentPlayHelpVoice_id:"+glCurrentPlayHelpVoice_id+ " glHelpVoiceQueue.length:"+glHelpVoiceQueue.length);
//
//     glHelpVoiceQueue.push(HelpVoiceItem);
//
//     play_HelpVoice();
// }
//
// function play_HelpVoice()
// {
//     console.log("play_HelpVoice() glCurrentPlayHelpVoice_id:"+glCurrentPlayHelpVoice_id+ " glHelpVoiceQueue.length:"+glHelpVoiceQueue.length);
//
//     var index = 0;
//     if (glHelpVoiceQueue.length > 0)
//     {
//         var HelpVoiceItem = glHelpVoiceQueue[index];
//
//         if (glCurrentPlayHelpVoice_id != HelpVoiceItem.HelpVoice_id)
//         {
//             if ( glCurrentPlayHelpVoice_id > 0 )
//             {
//                 // stop previous audio
//                 stop_HelpVoice(glCurrentPlayHelpVoice_id);
//                 glCurrentPlayHelpVoice_id = 0;
//             }
//
//             //
//             // play sound
//             //
//             if (HelpVoiceItem.HelpVoice_id == 1)
//             {
//                 HelpVoice_id = HelpVoiceAudio1.play();
//                 console.log("show_help_msg() HelpVoiceAudio1.play() HelpVoice_id:"+HelpVoice_id);
//             }
//             else
//             if (HelpVoiceItem.HelpVoice_id == 2)
//             {
//
//                 HelpVoice_id = HelpVoiceAudio2.play();
//                 console.log("show_help_msg() HelpVoiceAudio2.play() HelpVoice_id:"+HelpVoice_id);
//             }
//             glCurrentPlayHelpVoice_id = HelpVoiceItem.HelpVoice_id;
//         }
//     }
//     else
//     {
//         if ( glCurrentPlayHelpVoice_id > 0 )
//         {
//             // stop previous audio
//             stop_HelpVoice(glCurrentPlayHelpVoice_id);
//             glCurrentPlayHelpVoice_id = 0;
//         }
//     }
// }
//
// function stop_HelpVoice(HelpVoice_id)
// {
//     if (HelpVoice_id == 1)
//     {
//         console.log("stop_HelpVoice("+HelpVoice_id+") HelpVoiceAudio1.stop()");
//         HelpVoiceAudio1.stop();
//     }
//     else
//     if (HelpVoice_id == 2)
//     {
//         console.log("stop_HelpVoice("+HelpVoice_id+") HelpVoiceAudio2.stop()");
//         HelpVoiceAudio2.stop();
//     }
// }

function getCurrentTimeStamp()
{
    return Math.floor(Date.now() / 1000);
}
