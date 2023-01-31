<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link href="/css/vils.min.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Light Control Center</title>
    <style>

        body
        {
            font-size: 26px;
            font-weight: 700;
        }

        thead tr {
            background-color:rgb(236, 240, 245);
        }

        input[type=range]
        {
            width: 250px;
        }
        input[type=range]:last-child
        {
            margin-bottom: 10px;
        }

        .listItem {
            background-color: #FFF;
            cursor: pointer;
        }
        .listItemSelected {
            background-color: #CCEEFF;
        }

        #ui_power_switch {
            background-image: url(/images/power-off.svg);
            background-repeat: no-repeat;
            background-size: 48px;
            width: 48px;
            height: 48px;

        }
        .situation
        {
            display: block;
            width: 250px;
            height: 100px;
            line-height: 100px;
            margin: 10px 0px;
            border: 1px solid #c8c8c8;
            border-radius: 10px;
            background-color: #f3f1f1;
            cursor: pointer;;
            text-align: center;
            font-size: 28px;
            font-weight: 700;
        }

        .situation:hover, .targetTime:hover
        {
            border: 1px solid #F00;
        }

        .situationSelected
        {
            border: 3px solid #0F0;
            background-color: #FFF;
        }

        .targetTime
        {
            display: block;;
            /* width: 150px;
            height: 50px; */
            border: 1px solid #c8c8c8;
            border-radius: 10px;
            background-color: #CCEEFF;
            cursor: pointer;;
            text-align: center;
            /* line-height: 50px; */
            margin: 3px 5px;
            padding: 15px 0px;
        }

    </style>
  </head>

  <div class="box" >
      <div class="box-header with-border">
        <h3 class="box-title"><?php echo $vilslightdevicecontrol_title;?></h3>
        <div class="box-tools pull-right">
          <ul class="pagination pagination-sm no-margin pull-right">
            <li>
              <button type="button" class="btn btn-box-tool" onclick="open_lightdevicecontrol_page();return false;">
                <i class="fa fa-external-link"></i>
              </button>
            </li>
          </ul>
        </div>
      </div>
      <div class="box-body">
          <div class="row">
              <div class="col-xs-4 col-sm-3 col-md-3 col-lg-3">
                  <div class="nav-tabs-custom">
                    <ul class="nav nav-tabs">
                        <li class="active"><a href="#ls_tab_id" data-toggle="tab"><span class="badge bg-blue-active"><?php echo $vilslightdevicecontrol_light_tag;?></span></a></li>
                        <li class=""><a href="#lg_tab_id" data-toggle="tab"><span class="badge bg-blue-active"><?php echo $vilslightdevicecontrol_lightGroup_tag;?></span></a></li>
                        <li><a href="#pm_tab_search" data-toggle="tab">搜尋</a></li>
                    </ul>
                    <div class="tab-content">
                        <div class="tab-pane active table-responsive" id="ls_tab_id">
                            <div style="height:650px;overflow:scroll">
                               <table class="table table-hover">
                                  <thead>
                                      <tr style="background-color:#FFFFFF;">
                                        <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                        <th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilslightdevicecontrol_light_id;?></th>
                                      </tr>
                                  </thead>
                                  <tbody id="ls_list_content">
                                  </tbody>
                              </table>
                           </div>
                        </div>
                        <div class="tab-pane table-responsive" id="lg_tab_id">
                            <div style="height:650px;overflow:scroll">
                               <table class="table table-hover">
                                  <thead>
                                      <tr style="background-color:#FFFFFF;">
                                        <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                        <th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilslightdevicecontrol_lg_id;?></th>
                                      </tr>
                                  </thead>
                                  <tbody id="lg_list_content">
                                  </tbody>
                              </table>
                           </div>
                        </div>
                       <div class="tab-pane table-responsive" id="pm_tab_search">
                          <table class="table table-hover">
                              <thead>
                                  <input type="text" id="pm_tab_search_text">
                              </thead>
                              <tbody id="search_list_content" />
                          </table>
                       </div>
                    </div>
                  </div>
              </div>
              <div class="col-xs-8 col-sm-9 col-md-9 col-lg-9">
                  <div class="row">
                      <div class="col-xs-6 col-sm-6 col-md-6 col-lg-6">
                        <div id='light_controls' class="form-group">
                              <label>電源</label>
                              <div class="input-group">
                                <div class="btn btn-default pull-right" data-onOff='1' id="ui_power_switch"></div>
                              </div>
                            </div>
                            <label>亮度</label>
                            <div class="input-group">
                                <input type="range" id='ui_brightness' min="0" max="255" step="1" value="50" />
                            </div>
                            <label>色溫</label>
                            <div class="input-group" style="margin-bottom: 20px;">
                                <input type="range" id='ui_colorTemperature' min="0" max="255" step="1" value="50" />
                            </div>
                            <div class='input-group'>
                                <input id='ui_chk_brightness' type="checkbox" class='moveswitch' disabled=disabled />
                                <label for='ui_chk_brightness' style="margin-left:5px;">提升夜間褪黑激素濃度(1100Lx 5900K CAF[0.91])</label>
                                <div class='input-group' style="padding-left:20px;">
                                    <input type="text" id="rollcall_StartTime" readonly="readonly" class="form-control timepicker" value="07:00 AM">
                                    <div class="input-group-addon"><i class="fa fa-clock-o"></i></div>
                                </div>
                            </div>
                            <div class='input-group'>
                                <input id='ui_chk_color' type="checkbox" class='moveswitch' disabled=disabled />
                                <label for='ui_chk_color' style="margin-left:5px;">放鬆舒壓(550Lx 4500K CAF[0.63])</label>
                                <div class='input-group' style="padding-left:20px;">
                                    <input type="text" id="rollcall_StartTime" readonly="readonly" class="form-control timepicker" value="12:00 PM">
                                    <div class="input-group-addon"><i class="fa fa-clock-o"></i></div>
                                </div>
                            </div>
                            <div class='input-group'>
                                <input id='ui_chk_breathe' type="checkbox" class='moveswitch' disabled=disabled />
                                <label for='ui_chk_breathe' style="margin-left:5px;">促進睡意(200Lx 3000K CAF[0.45])</label>
                                <div class='input-group' style="padding-left:20px;">
                                    <input type="text" id="rollcall_StartTime" readonly="readonly" class="form-control timepicker" value="08:00 PM">
                                    <div class="input-group-addon"><i class="fa fa-clock-o"></i></div>
                                </div>
                            </div>
                            <div class='row' style="padding: 30px 0px;">
                                <div class="col-md-12 targetTime" data-key='morning'>早晨<font style="font-size:16px;">(1100Lx 5900K CAF[0.91])</font></div>
                                <div class="col-md-12 targetTime" data-key='noon'>中午<font style="font-size:16px;">(550Lx 4500K CAF[0.63])</font></div>
                                <div class="col-md-12 targetTime" data-key='evening'>傍晚<font style="font-size:16px;">(200Lx 3000K CAF[0.45])</font></div>
                            </div>
                      </div>
                      <div class="col-xs-6 col-sm-6 col-md-6 col-lg-6">
                        <div id='light_situation' class="form-group">
                            <div class="situation" id="div_bright">亮度漸變</div>
                            <div class="situation" id="div_color">色溫漸變</div>
                            <div class="situation" id="div_breathe">呼吸燈</div>
                        </div>
                      </div>
                  </div>
              </div>
      </div>
      </div>
  </div>

  <script>
    var project_id = <?php echo $project_id ?>;
    var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
    var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
    var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
  </script>

  <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/lightdevicecontrol_src/00-lightdevicecontrol.js?s=1"></script>

</html>
