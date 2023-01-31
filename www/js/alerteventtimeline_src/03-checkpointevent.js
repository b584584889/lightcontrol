/*
 * Author: ikki Chung
 * Date: 30 Jan 2019
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


function show_checkpoint_alert_msg(chcekPointName, alertType)
{
    var showMsg = " alert ";
    if ( parseInt(alertType) == 2)
    {
        showMsg = chcekPointName + " 巡檢時間不足!";
    }
    else
    if ( parseInt(alertType) == 3)
    {
        showMsg = chcekPointName + " 巡檢時間過長!";
    }
    else
    if ( parseInt(alertType) == 4)
    {
        showMsg = chcekPointName + " 巡檢時間已過, 未見巡檢員!";
    }

    //
    // Alert box
    //
    create_alert_box(chcekPointName, chcekPointName, showMsg, 6);
}
