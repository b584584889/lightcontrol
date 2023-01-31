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
    <title>SMIMS VILS IoT - Setting</title>
  </head>


        <div class="row">

            <!-- left column -->
            <div class="col-md-3">
                <!-- general form elements -->
                <div class="box box-primary">
                    <div class="box-header with-border">
                        <h3 class="box-title"><?php echo $vilssetting_title;?></h3>
                        <div class="box-tools pull-right">
                            <button type="button" class="btn btn-box-tool" onclick="open_settings_page();return false;"><i class="fa fa-external-link"></i>
                            </button>
                            <!-- <a href="javascript:void(0)" onclick="open_settings_page();return false;"><i class="fa fa-external-link"></i></a> -->
                        </div>
                    </div>
                    <!-- /.box-header -->
                    <form id="configformid">
                      <div class="box-body">
                        <div class="form-group">
                          <label for="Local_Project_Name">Project Name</label>
                          <input type="text" class="form-control" name="Local_Project_Name" id="Local_Project_Name" value="Project Name">
                        </div>
                        <div class="form-group">
                          <label for="Local_Project_Desc">Project Description</label>
                          <input type="text" class="form-control" name="Local_Project_Desc" id="Local_Project_Desc" value="">
                        </div>
                        <div class="form-group" style="display:none">
                          <label for="WGS48OriginX">Origin WGS48 X value</label>
                          <input type="text" class="form-control" name="WGS48OriginX" id="WGS48OriginX" value="121.5379560">
                        </div>
                        <div class="form-group" style="display:none">
                          <label for="WGS48OriginY">Origin WGS48 Y value</label>
                          <input type="text" class="form-control" name="WGS48OriginY" id="WGS48OriginY" value="25.0401453">
                        </div>
                        <div class="form-group" style="display:none">
                          <label for="MAP_North">MAP North Direction(Degree:0 for UP, Degree:90 for right)</label>
                          <input type="text" class="form-control" name="MAP_North" id="MAP_North" value="0.0">
                        </div>
                        <div class="form-group" style="display:none">
                          <label for="BuildingOriginX">Building Origin X value</label>
                          <input type="text" class="form-control" name="BuildingOriginX" id="BuildingOriginX" value="121.5379560">
                        </div>
                        <div class="form-group" style="display:none">
                          <label for="BuildingOriginY">Building Origin Y value</label>
                          <input type="text" class="form-control" name="BuildingOriginY" id="BuildingOriginY" value="25.0401453">
                        </div>
                        <div class="form-group" style="display:none">
                          <label for="BuildingXLength">Building X-Length value</label>
                          <input type="text" class="form-control" name="BuildingXLength" id="BuildingXLength" value="0.0001">
                        </div>
                        <div class="form-group" style="display:none">
                          <label for="BuildingYLength">Building Y-Length value</label>
                          <input type="text" class="form-control" name="BuildingYLength" id="BuildingYLength" value="0.0001">
                        </div>
                        <div class="form-group" style="display:none">
                          <label for="BuildingID"><?php echo $vilssetting_building;?></label>
                          <input type="text" class="form-control" name="BuildingID" id="BuildingID" value="15">
                        </div>
                        <div class="form-group" style="display:none">
                          <label for="BuildingFloor"><?php echo $vilssetting_floor;?></label>
                          <input type="text" class="form-control" name="BuildingFloor" id="BuildingFloor" value="1">
                        </div>
                      </div>
                      <!-- /.box-body -->
                      <div class="box-footer">
                          <div class="callout callout-warning" id="vilssetting_warning_callout" style="display:none">
                            <p>Updating...</p>
                          </div>
                          <div class="callout callout-success" id="vilssettingupdate_callout" style="display:none">
                            <p>Update Successfully!</p>
                          </div>
                        <button type="submit" class="btn btn-danger pull-left" id="deletethisprojectbtn">Delete this project</button>
                        <button type="submit" class="btn btn-info pull-center" id="restartappbtn"><?php echo $vilssetting_restart;?></button>
                        <button type="submit" class="btn btn-primary pull-right" id="prjsavebtn"><?php echo $vilssetting_save;?></button>
                      </div>
                    </form>

                </div>
                <!-- /.box -->
            </div>
            <!--/.col (left) -->


            <!-- right column -->
            <div class="col-md-4">
                <!-- general form elements -->
                <div class="box box-primary">
                    <div class="box-header with-border">
                        <h3 class="box-title">VILS Center Setting</h3>
                        <div class="box-tools pull-right">
                            <button type="button" class="btn btn-box-tool" onclick="open_settings_page();return false;"><i class="fa fa-external-link"></i>
                            </button>
                            <!-- <a href="javascript:void(0)" onclick="open_settings_page();return false;"><i class="fa fa-external-link"></i></a> -->
                        </div>
                    </div>
                    <!-- /.box-header -->
                    <form id="systemconfigformid">
                      <div class="box-body">
                        <div class="form-group">
                          <label>VILS Server (需要http://或https://)</label>
                          <input type="text" class="form-control" id="VILSServerInputText" value="">
                        </div>
                        <div class="form-group">
                          <label>ERP Server</label>
                          <input type="text" class="form-control" id="ERPServerInputText" value="">
                        </div>
                        <div class="form-group">
                          <label>SSO(single sign on) Server IP</label>
                          <input type="text" class="form-control" id="SSOIPInputText" value="">
                        </div>
                        <!-- <div class="form-group">
                          <label>Push Notification Prefix</label>
                          <input type="text" class="form-control" id="PushNotifyPrefixInputText" value="">
                          <div class="checkbox">
                            <label>
                              <input type="checkbox" id="EnablePushNotifyCheckBtn"> Enable Push Notification
                            </label>
                          </div>
                        </div> -->
                        <div class="form-group">
                          <label>Center App Web API UDP Port</label>
                          <input type="text" class="form-control" id="CenterWebPortInputText" value="18613">
                        </div>
                        <div class="form-group">
                          <label>Center App UDP Port</label>
                          <input type="text" class="form-control" id="CenterUdpPortInputText" value="18614">
                        </div>
                        <div class="form-group">
                          <label>Coordinator App UDP Port</label>
                          <input type="text" class="form-control" id="CoordUdpPortInputText" value="18615">
                        </div>
                        <div class="form-group">
                          <label>Center App Broadcast Port</label>
                          <input type="text" class="form-control" id="CenterBroadcastPortInputText" value="12317">
                        </div>
                        <div class="form-group">
                          <label>Center App WebSocket API Port</label>
                          <input type="text" class="form-control" id="CenterWebSocketPortInputText" value="8083">
                        </div>
                        <div class="form-group">
                          <label>Tag Alert timeout(ms)</label>
                          <input type="text" class="form-control" id="TagAlertTimeoutInputText" value="5000">
                        </div>
                        <div class="form-group">
                          <label>Tag voltage low alert(V)(伏特)</label>
                          <input type="text" class="form-control" id="TagVoltageLowInputText" value="3.8">
                        </div>
                       </div>
                      <!-- /.box-body -->
                      <div class="box-footer">
                          <div class="callout callout-warning" id="systemconfig_warning_callout" style="display:none">
                            <p>Updating...</p>
                          </div>
                          <div class="callout callout-success" id="systemconfigupdate_callout" style="display:none">
                            <p>Update Successfully!</p>
                          </div>

                        <button type="submit" class="btn btn-primary pull-right" id="systemconfigupdatebtn">Update</button>
                        <!-- textarea -->
                          <label>System Status</label>
                          <textarea class="form-control" rows="11" placeholder="" id="systemstatusarea"></textarea>
                      </div>
                    </form>

                </div>
                <!-- /.box -->
            </div>
            <!--/.col (right) -->

            <!-- column -->
            <div class="col-md-5">
                <div class="box box-primary">
                    <div class="box-header with-border">
                    <h3 class="box-title">VILS Center Algorithm Setting</h3>
                    <div class="box-tools pull-right">
                        <button type="button" class="btn btn-box-tool" onclick="open_settings_page();return false;"><i class="fa fa-external-link"></i>
                        </button>
                        <!-- <a href="javascript:void(0)" onclick="open_settings_page();return false;"><i class="fa fa-external-link"></i></a> -->
                    </div>

                    </div>
                    <!-- /.box-header -->
                    <form id="algorithmconfigformid">
                    <div class="box-body">
                      <div class="box-group" id="accordion">
                        <!-- we are adding the .panel class so bootstrap.js collapse plugin detects it -->
                        <div class="panel box box-default">
                          <div class="box-header with-border">
                            <h4 class="box-title">
                              <a data-toggle="collapse" data-parent="#accordion" href="#collapseOne">
                                1.Ranging Data Preprocessing
                              </a>
                            </h4>
                          </div>
                          <div id="collapseOne" class="panel-collapse collapse in">
                            <div class="box-body">
                                <ul class="list-unstyled">

                                    <li>
                                        <div class="form-group">
                                        <label>Valid data time (ms)</label>
                                        <input type="text" class="form-control" id="ValidDataTimeInputText" value="1000">
                                        </div>
                                    </li>

                                    <li>
                                        <div class="form-group">
                                        <label>MAX distance for tag and anchor ranging (cm)</label>
                                        <input type="text" class="form-control" id="MaxRangingDistanceInputText" value="3500">
                                        </div>
                                    </li>

                                  <li>
                                      <div class="checkbox">
                                        <label>
                                          <input type="checkbox" id="NoiseMakupCheckBtn" onclick="NoiseMakupCheckBtn_onclick();"> Enable NOISE_MAKEUP
                                        </label>
                                        <ul id="NoiseMakupCheckBtn_ul" style="display:none">
                                            <li>
                                              <label>實際測距值與平均測距值比例, 大於此數時計算合理測距值取代目前實際測距值</label>
                                              <input type="text" class="form-control" id="NoiseMakupRatioInputText" value="2.0">
                                            </li>
                                            <li>
                                              <label>實際測距值併入平均測距值的比重.(0.1~1.0)</label>
                                              <input type="text" class="form-control" id="NoiseMakupWeightInputText" value="0.8">
                                            </li>
                                        </ul>
                                      </div>
                                      <!-- radio -->
                                      <div class="form-group">
                                        <div class="checkbox">
                                          <label>
                                            <input type="checkbox" id="EnableKalman1DCheckBtn"> Enable KALMAN 1D
                                          </label>
                                        </div>
                                        <div class="checkbox">
                                          <label>
                                            <input type="checkbox" id="EnableRangingReliabilityCheckBtn" onclick="EnableRangingReliabilityCheckBtn_onclick();"> Enable Ranging Reliability
                                          </label>
                                          <ul id="RangingReliabilityInputText_ul" style="display:none">
                                              <li>
                                                <label>最大可用測距值(cm), 大於此數將捨棄.</label>
                                                <input type="text" class="form-control" id="RangingReliabilityMaxDistanceInputText" value="1300">
                                              </li>
                                              <li>
                                                <label>特定時間內至少可用測距次數, 少於此數將捨棄.</label>
                                                <input type="text" class="form-control" id="RangingReliabilityMinCountInputText" value="1">
                                              </li>
                                              <li>
                                                <label>最大平均測距差(cm), 大於此數將捨棄.</label>
                                                <input type="text" class="form-control" id="RangingReliabilityAverageDiffInputText" value="50">
                                              </li>
                                          </ul>
                                        </div>
                                      </div>
                                  </li>
                                </ul>
                            </div>
                          </div>
                        </div>
                        <div class="panel box box-danger">
                          <div class="box-header with-border">
                            <h4 class="box-title">
                              <a data-toggle="collapse" data-parent="#accordion" href="#collapseTwo">
                                2.Position Calculation
                              </a>
                            </h4>
                          </div>
                          <div id="collapseTwo" class="panel-collapse collapse">
                            <div class="box-body">
                                <ul class="list-unstyled">
                                  <li>
                                      <div class="checkbox">
                                        <label>
                                          <input type="checkbox" id="Anchor1DMappingCheckBtn"> Enable ANCHOR_1D_MAPPING at 1D Section
                                        </label>
                                      </div>
                                  </li>

                                  <li>
                                      <div class="checkbox">
                                        <label>
                                          <input type="checkbox" id="OneAnchorCalculationCheckBtn"> Enable One Anchor Calculation at 1D Section
                                        </label>
                                      </div>
                                  </li>

                                  <li>
                                      <div class="checkbox">
                                        <label>
                                          <input type="checkbox" id="GDOPresolveCheckBtn" onclick="GDOPresolveCheckBtn_onclick();"> Enable GDOP (Geometric Dilution of Precision)
                                        </label>
                                        <ul id="GDOPresolveInputText_ul" style="display:none">
                                            <li>
                                              <label>GDOP THRESHOLD</label>
                                              <input type="text" class="form-control" id="GDOPThresholdInputText" value="60">
                                            </li>
                                        </ul>
                                      </div>
                                  </li>

                                  <li>
                                      <div class="checkbox">
                                        <label>
                                          <input type="checkbox" id="IMUVelocityCheckBtn" onclick="IMUVelocityCheckBtn_onclick();"> Enable IMU Velocity fix large drift position
                                        </label>
                                        <ul id="IMUVelocityOffsetInputText_ul" style="display:none">
                                            <li>
                                              <label>Velocity Offet(Velocity * Offet)</label>
                                              <input type="text" class="form-control" id="IMUVelocityOffsetInputText" value="100">
                                            </li>
                                        </ul>
                                      </div>
                                  </li>

                                    <li>
                                        <div class="checkbox">
                                          <label>
                                            <input type="checkbox" id="EnableFixHeightCheckBtn"> Enable Fix Height (Based on Anchor/Locator Height)
                                          </label>
                                        </div>
                                    </li>

                                  <li>
                                      <label>Steady State Offset(cm), When IMU Velocity <= 0.1 m/s</label>
                                      <input type="text" class="form-control" id="SteadyStateOffsetInputText" value="5">
                                  </li>

                                    <li>
                                        <label>Stay In Wall Timeout(ms), 停留多少時間在牆上時, 讓Tag穿牆.</label>
                                        <input type="text" class="form-control" id="StayInWallTimeoutInputText" value="5000">
                                    </li>

                                  <li>
                                      <!-- radio -->
                                      <div class="form-group">
                                        <div class="radio">
                                          <label>
                                            <input type="radio" name="optionsRadioPositionAlgorithms" id="optionsPositionAlgorithm1" value="KALMAN_FILTER"  onclick="optionsPositionAlgorithm1_onclick();" checked>
                                            Apply KALMAN Filter
                                          </label>
                                        </div>
                                        <div class="radio">
                                          <label>
                                            <input type="radio" name="optionsRadioPositionAlgorithms" id="optionsPositionAlgorithm2" value="SMOOTHING_FILTER" onclick="optionsPositionAlgorithm2_onclick();">
                                            Apply SMOOTHING Filter
                                          </label>
                                        </div>
                                        <div class="radio">
                                          <label>
                                            <input type="radio" name="optionsRadioPositionAlgorithms" id="optionsPositionAlgorithm3" value="WMOVINGAVERAGE_FILTER" onclick="optionsPositionAlgorithm3_onclick();">
                                            Apply Weight Moving Average Filter
                                          </label>
                                          <ul id="optionsPositionAlgorithm3_ul" style="display:none">
                                              <li>
                                                <label>參數1</label>
                                                <input type="text" class="form-control" id="WeightMovingAverageInputText1" value="0.01">
                                              </li>
                                              <li>
                                                <label>參數2</label>
                                                <input type="text" class="form-control" id="WeightMovingAverageInputText2" value="0.02">
                                              </li>
                                              <li>
                                                <label>參數3</label>
                                                <input type="text" class="form-control" id="WeightMovingAverageInputText3" value="0.97">
                                              </li>
                                          </ul>
                                        </div>
                                      </div>
                                  </li>
                                </ul>

                            </div>
                          </div>
                        </div>
                        <div class="panel box box-success">
                          <div class="box-header with-border">
                            <h4 class="box-title">
                              <a data-toggle="collapse" data-parent="#accordion" href="#collapseThree">
                                3.Position Result
                              </a>
                            </h4>
                          </div>
                          <div id="collapseThree" class="panel-collapse collapse">
                            <div class="box-body">
                                <div class="form-group">
                                    <div class="checkbox">
                                      <label>
                                        <input type="checkbox" id="SmoothingResultCheckBtn" onclick="SmoothingResultCheckBtn_onclick();"> Enable Smoothing Result
                                      </label>
                                    </div>
                                  <ul id="SmoothingResultOffsetInputText_ul" style="display:none">
                                      <li>
                                          <label>Smoothing Result Minimal Offset(cm), 小於此數的位移將不做補點.</label>
                                          <input type="text" class="form-control" id="SmoothingResultOffsetInputText" value="10">
                                      </li>
                                      <li>
                                          <label>Smoothing Result Steps.</label>
                                          <input type="text" class="form-control" id="SmoothingResultStepInputText" value="6">
                                      </li>
                                      <li>
                                        <label>First State Offset Ratio(0.1~1.0)</label>
                                        <input type="text" class="form-control" id="SmoothingResultFirstStateOffsetRatioInputText" value="0.3">
                                      </li>
                                      <li>
                                        <label>First State Run Time(ms)</label>
                                        <input type="text" class="form-control" id="SmoothingResultFirstStateRunTimeInputText" value="200">
                                      </li>
                                      <li>
                                        <label>Second State Offset Ratio(0.1~1.0)</label>
                                        <input type="text" class="form-control" id="SmoothingResultSecondStateOffsetRatioInputText" value="0.8">
                                      </li>
                                      <li>
                                        <label>Second State Run Time(ms)</label>
                                        <input type="text" class="form-control" id="SmoothingResultSecondStateRunTimeInputText" value="600">
                                      </li>
                                  </ul>
                                </div>
                            </div>
                          </div>
                        </div>
                      </div>
                    </div>
                    </form>
                    <!-- /.box-body -->
                    <div class="box-footer">
                        <div class="callout callout-warning" id="algorithmconfig_warning_callout" style="display:none">
                          <p>Updating...</p>
                        </div>
                        <div class="callout callout-success" id="algorithmconfigupdate_callout" style="display:none">
                          <p>Update Successfully!</p>
                        </div>
                      <button type="submit" class="btn btn-primary pull-right" id="algorithmconfigupdatebtn">Update</button>
                    </div>
                </div>
                <!-- /.box -->
            </div>
            <!--/.col -->

        </div>
        <!--/.row -->

        <div class="row">

            <!-- column -->
            <div class="col-md-4">
                <!-- general form elements -->
                <div class="box box-primary">
                    <div class="box-header with-border">
                        <h3 class="box-title">VILS Center LoRAWAN Setting</h3>
                        <div class="box-tools pull-right">
                            <button type="button" class="btn btn-box-tool" onclick="open_settings_page();return false;"><i class="fa fa-external-link"></i>
                            </button>
                            <!-- <a href="javascript:void(0)" onclick="open_settings_page();return false;"><i class="fa fa-external-link"></i></a> -->
                        </div>
                    </div>
                    <!-- /.box-header -->
                    <form id="lorawanconfigformid">
                      <div class="box-body">
                        <div class="form-group">
                          <label>LoRAWAN TOPIC</label>
                          <input type="text" class="form-control" id="LoRAWANTPOICInputText" value="GIOT-GW/UL/#">
                        </div>
                        <div class="form-group">
                          <label>MQTT HOST IP</label>
                          <input type="text" class="form-control" id="MQTTHostIpInputText" value="192.168.99.103">
                        </div>
                        <div class="form-group">
                          <label>MQTT HOST PORT</label>
                          <input type="text" class="form-control" id="MQTTHostPortInputText" value="1883">
                        </div>
                        <div class="form-group">
                          <label>MQTT HOST USERNAME</label>
                          <input type="text" class="form-control" id="MQTTHostUsernameInputText" value="admin">
                        </div>
                        <div class="form-group">
                          <label>MQTT HOST PASSWORD</label>
                          <input type="text" class="form-control" id="MQTTHostPasswordInputText" value="smims2017">
                        </div>
                        <div class="form-group">
                          <label>MQTT CLIENT ID</label>
                          <input type="text" class="form-control" id="MQTTClientIdInputText" value="200cecd38e824e6388827552939fe4d1">
                        </div>

                      </div>
                      <!-- /.box-body -->
                      <div class="box-footer">
                          <div class="callout callout-warning" id="lorawanconfig_warning_callout" style="display:none">
                            <p>Updating...</p>
                          </div>
                          <div class="callout callout-success" id="lorawanconfigupdate_callout" style="display:none">
                            <p>Update Successfully!</p>
                          </div>
                        <button type="submit" class="btn btn-danger pull-left" id="restlorabtn">Reset LoRA</button>
                        <button type="submit" class="btn btn-primary pull-right" id="lorawanconfigupdatebtn">Update</button>
                      </div>
                    </form>

                </div>
                <!-- /.box -->
            </div>
            <!--/.col -->

        </div>
        <!--/.row -->


  <script>
    var project_id = <?php echo $project_id ?>;
    var accountid = "<?php echo $_SESSION["accountid"] ?>";
    var accountname = "<?php echo $_SESSION["accountname"] ?>";
    var groupid = "<?php echo $_SESSION["groupid"] ?>";
  </script>



    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/settings.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/settings_src/00-settings.js"></script> -->

</html>
