/*
 * Author: ikki Chung
 * Date: 20 June 2019
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


 var glHelpVoiceQueue = [];
 var glCurrentPlayHelpVoice_id = -1;

 var HelpVoiceAudio1 = new Howl({
   src: ['/audio/人員區域求助.mp3'],
   autoplay: false,
   loop: true,
   html5: true
 });

 var HelpVoiceAudio2 = new Howl({
   src: ['/audio/固定裝置求助.mp3'],
   autoplay: false,
   loop: true,
   html5: true
 });

 var FenceAlertHelpVoiceAudio1 = new Howl({
   src: ['/audio/人員進入電子圍欄.mp3'],
   autoplay: false,
   loop: true,
   html5: true
 });

 var FenceAlertHelpVoiceAudio2 = new Howl({
   src: ['/audio/人員離開電子圍欄.mp3'],
   autoplay: false,
   loop: true,
   html5: true
 });

 function create_HelpVoice(nodeid, new_HelpVoice_id)
 {
     var newHelpVoiceItem = {};
     newHelpVoiceItem.nodeid = nodeid;
     newHelpVoiceItem.HelpVoice_id = new_HelpVoice_id;

     console.log("create_HelpVoice() nodeid:"+nodeid+" glCurrentPlayHelpVoice_id:"+glCurrentPlayHelpVoice_id+ " glHelpVoiceQueue.length:"+glHelpVoiceQueue.length);

     if ( new_HelpVoice_id == 3 || new_HelpVoice_id == 4 )
     {
         // 進入/離開 電子圍欄, 只保留最新的.
         for(var index = 0; index<glHelpVoiceQueue.length;index++)
         {
             var HelpVoiceItem = glHelpVoiceQueue[index];
             if ( HelpVoiceItem.HelpVoice_id == 3 || HelpVoiceItem.HelpVoice_id == 4 )
             {
                 glHelpVoiceQueue.splice(index, 1);
                 break;
             }
         }
     }

     glHelpVoiceQueue.push(newHelpVoiceItem);

     play_HelpVoice();
 }

 function remove_HelpVoice(nodeid)
 {
     for(var index = 0; index<glHelpVoiceQueue.length;index++)
     {
         var HelpVoiceItem = glHelpVoiceQueue[index];
         if ( HelpVoiceItem.nodeid === nodeid )
         {
             glHelpVoiceQueue.splice(index, 1);
             break;
         }
     }

     play_HelpVoice();
 }

 function play_HelpVoice()
 {
     console.log("play_HelpVoice() glCurrentPlayHelpVoice_id:"+glCurrentPlayHelpVoice_id+ " glHelpVoiceQueue.length:"+glHelpVoiceQueue.length);

     var index = 0;
     if (glHelpVoiceQueue.length > 0)
     {
         var HelpVoiceItem = glHelpVoiceQueue[index];

         if (glCurrentPlayHelpVoice_id != HelpVoiceItem.HelpVoice_id)
         {
             if ( glCurrentPlayHelpVoice_id > 0 )
             {
                 // stop previous audio
                 stop_HelpVoice(glCurrentPlayHelpVoice_id);
                 glCurrentPlayHelpVoice_id = 0;
             }

             //
             // play sound
             //
             if (HelpVoiceItem.HelpVoice_id == 1)
             {
                 HelpVoice_id = HelpVoiceAudio1.play();
                 console.log("play_HelpVoice() HelpVoiceAudio1.play() HelpVoice_id:"+HelpVoice_id);
             }
             else
             if (HelpVoiceItem.HelpVoice_id == 2)
             {
                 HelpVoice_id = HelpVoiceAudio2.play();
                 console.log("play_HelpVoice() HelpVoiceAudio2.play() HelpVoice_id:"+HelpVoice_id);
             }
             else
             if (HelpVoiceItem.HelpVoice_id == 3)
             {
                 HelpVoice_id = FenceAlertHelpVoiceAudio1.play();
                 console.log("play_HelpVoice() FenceAlertHelpVoiceAudio1.play() HelpVoice_id:"+HelpVoice_id);
             }
             else
             if (HelpVoiceItem.HelpVoice_id == 4)
             {
                 HelpVoice_id = FenceAlertHelpVoiceAudio2.play();
                 console.log("play_HelpVoice() FenceAlertHelpVoiceAudio2.play() HelpVoice_id:"+HelpVoice_id);
             }

             glCurrentPlayHelpVoice_id = HelpVoiceItem.HelpVoice_id;
         }
     }
     else
     {
         if ( glCurrentPlayHelpVoice_id > 0 )
         {
             // stop previous audio
             stop_HelpVoice(glCurrentPlayHelpVoice_id);
             glCurrentPlayHelpVoice_id = 0;
         }
     }
 }

 function stop_HelpVoice(HelpVoice_id)
 {
     if (HelpVoice_id == 1)
     {
         console.log("stop_HelpVoice("+HelpVoice_id+") HelpVoiceAudio1.stop()");
         HelpVoiceAudio1.stop();
     }
     else
     if (HelpVoice_id == 2)
     {
         console.log("stop_HelpVoice("+HelpVoice_id+") HelpVoiceAudio2.stop()");
         HelpVoiceAudio2.stop();
     }
     else
     if (HelpVoice_id == 3)
     {
         console.log("stop_HelpVoice("+HelpVoice_id+") FenceAlertHelpVoiceAudio1.stop()");
         FenceAlertHelpVoiceAudio1.stop();
     }
     else
     if (HelpVoice_id == 4)
     {
         console.log("stop_HelpVoice("+HelpVoice_id+") FenceAlertHelpVoiceAudio2.stop()");
         FenceAlertHelpVoiceAudio2.stop();
     }
 }
