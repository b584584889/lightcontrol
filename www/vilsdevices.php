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
    <title>SMIMS VILS IoT - Devices</title>
  </head>

          <div class="modal fade" id="modal-qrcode">
            <div class="modal-dialog">
              <div class="modal-content">
                <div class="modal-header">
                  <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                    <span aria-hidden="true">&times;</span></button>
                  <h4 class="modal-title">QR Code</h4>
                </div>
                <div class="modal-body">
                  <div id='device_qrcode_type'></div>
                  <div id='device_qrcode_name'></div>
                  <div id='device_qrcode_macaddress'></div>
                  <div id='device_qrcode_target'></div>
                  <div id='device_qrcode_topic'></div>
                  <div id='device_qrcode_datetime'></div>
                  <img src='' alt='' class='margin'width='250' id='device_qrcode_ui'>
                </div>
                <div class="modal-footer">
                  <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
                </div>
              </div>
              <!-- /.modal-content -->
            </div>
            <!-- /.modal-dialog -->
          </div>
          <!-- /.modal -->

          <div class="modal fade" id="modal-atcommand">
            <div class="modal-dialog">
              <div class="modal-content">
                <div class="modal-header">
                  <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                    <span aria-hidden="true">&times;</span></button>
                  <h4 class="modal-title">AT Command</h4>
                </div>
                <div class="modal-body">
                  <div >發送對象</div>
                  <div id='device_name_atcmd'></div>
                  <div id='device_macaddress_atcmd'></div>
                  <div id='device_version_atcmd'></div>
                  <div id='device_panid_atcmd'></div>
                  <div id='device_mode_atcmd'></div>

                  <div>
                    <div>
                      <label class="control-label">預設指令:</label>
                      <div class="btn-group">
                        <button type="button" class="btn btn-default btn-flat">AT Command</button>
                        <button type="button" class="btn btn-default btn-flat dropdown-toggle" data-toggle="dropdown">
                          <span class="caret"></span>
                          <span class="sr-only">Toggle Dropdown</span>
                        </button>
                        <ul class="dropdown-menu" role="menu">
                          <li><a href="javascript:;" onclick="dropdown_atcmd_click('AT+CCHECK');">檢查燈號(閃三下)</a></li>
                          <li><a href="javascript:;" onclick="dropdown_atcmd_click('AT+SEMERMSG=?');">檢查按鈕功能</a></li>
                          <li><a href="javascript:;" onclick="dropdown_atcmd_click('AT+SRETRY=?');">檢查重送次數</a></li>
                          <li><a href="javascript:;" onclick="dropdown_atcmd_click('AT+CRST');">重啟</a></li>
                          <li><a href="javascript:;" onclick="dropdown_atcmd_click('AT+VERSION');">檢查版本號</a></li>
                          <li><a href="javascript:;" onclick="dropdown_atcmd_click('AT+SEMERMSG=?');">檢查SOS按鈕狀態[ON/OFF]</a></li>
                          <li><a href="javascript:;" onclick="dropdown_atcmd_click('AT+CPWRST');">PowerSwicth電源開關(關閉再開啟)</a></li>
                        </ul>
                      </div>
                    </div>
                  </div>

                  <div>
                      <label for="inputAtcmd" class="col-sm-1 control-label" style="width:80px;">指令:</label>
                      <input type="text" class="form-control" id="inputAtcmd">
                  </div>

                </div>
                <div class="modal-footer">
                    <div class="callout callout-warning" id="sendAtcmd_sending_callout" style="display:none">
                      <p>發送指令中...</p>
                    </div>
                    <div class="callout callout-danger" id="sendAtcmd_failed_callout" style="display:none">
                      <div id="sendAtcmd_failed_msg_callout"><p>發送指令 失敗!</p></div>
                    </div>
                    <div class="callout callout-success" id="sendAtcmd_finished_callout" style="display:none">
                      <p>發送指令 成功!</p>
                    </div>
                    <div class="callout callout-info" id="sendAtcmd_reponse_msg_callout" style="display:none">
                      <p></p>
                    </div>

                  <button type="button" class="btn btn-default" id="sendAtcmdbtn">發送指令</button>
                </div>
              </div>
              <!-- /.modal-content -->
            </div>
            <!-- /.modal-dialog -->
          </div>
          <!-- /.modal -->

                <div class="box">
                  <div class="box-header with-border">
                    <h3 class="box-title"><?php echo $vilsdevices_devicelist;?></h3>
                    <div class="box-tools pull-right">
                        <div class="btn-group">
                            <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown"><?php echo $vilsdevices_showfilter;?><span class="caret"></span></button>
                            <ul class="dropdown-menu" id="dropdown_filter">
                                <li>
                                    <a href="javascript:;" class="small" tabIndex="-1" data-value="show_online" onclick="dropdown_filter_click('show_online');return true;">
                                        <input type="checkbox" id="dropdown_checkbox_filter_show_online" onclick="dropdown_checkbox_filter_click('show_online');return true;" checked="true">
                                        </input>
                                        <?php echo $vilsdevices_filter_showonline;?>
                                    </a>
                                </li>
                                <li>
                                    <a href="javascript:;" class="small" tabIndex="-1" data-value="show_offline" onclick="dropdown_filter_click('show_offline');return true;">
                                        <input type="checkbox" id="dropdown_checkbox_filter_show_offline" onclick="dropdown_checkbox_filter_click('show_offline');return true;" checked="true">
                                        </input>
                                        <?php echo $vilsdevices_filter_showoffline;?>
                                    </a>
                                </li>
                            </ul>
                        </div>

                      <!-- <a href="javascript:void(0)" onclick="open_dvices_page();return false;"><i class="fa fa-external-link"></i></a> -->
                      <!-- <a href="#control-sidebar-settings-tab" data-toggle="control-sidebar"><i class="fa fa-gears"></i></a> -->
                      <button type="button" class="btn btn-box-tool" onclick="open_dvices_page();return false;"><i class="fa fa-external-link"></i>
                      </button>

                      <?php
                          if ($GL_CONFIG_EDIT_NODELIST == 1 )
                          {
                              echo "    <button type='button' class='btn btn-box-tool' data-toggle='control-sidebar' onclick='toggleOn();'><i class='fa fa-gears'></i>";
                              echo "    </button>";
                          }
                      ?>

                      <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
                      </button>
                    </div>
                  </div>
                  <!-- /.box-header -->

                  <div class="box-body">
                      <!-- Custom Tabs (Pulled to the right) -->
                      <div class="nav-tabs-custom">
                        <ul class="nav nav-tabs">
                            <?php
                                //if ($_SESSION['type'] == 2 )
                                // if ($_SESSION["groupid"] == 5 || $_SESSION["groupid"] == 6 )
                                // {
                                //     echo "<li class='active'><a href=\"#loc_tab_nfclocators\" data-toggle='tab' onclick=\"device_tab_onclick('nfclocators');\"><span class='badge bg-yellow'>"; echo $vilsdevices_nfclocator;echo "</span></a></li>\n";
                                //     echo "<li><a href=\"#loc_tab_locators\" data-toggle='tab' onclick=\"device_tab_onclick('locators');\"><span class='badge bg-yellow-active'>"; echo $vilsdevices_locator; echo "</span></a></li>";
                                //     echo "<li><a href=\"#loc_tab_celllocators\" data-toggle='tab' onclick=\"device_tab_onclick('celllocators');\"><span class='badge bg-yellow-active'>"; echo $vilsdevices_celllocator; echo "</span></a></li>";
                                //     echo "<li><a href=\"#loc_tab_wifilocators\" data-toggle='tab' onclick=\"device_tab_onclick('wifilocators');\"><span class='badge bg-yellow-active'>"; echo $vilsdevices_wifilocator; echo "</span></a></li>";
                                //     echo "<li><a href=\"#loc_tab_edgecontrols\" data-toggle='tab' onclick=\"device_tab_onclick('edgecontrols');\"><span class='badge bg-yellow-active'>"; echo $vilsdevices_edgecontrol; echo "</span></a></li>";
                                //     echo "<li><a href=\"#loc_tab_nfccards\" data-toggle='tab' onclick=\"device_tab_onclick('nfccards');\"><span class='badge bg-blue'>"; echo $vilsdevices_nfccard; echo "</span></a></li>";
                                //     echo "<li><a href=\"#loc_tab_sensors\" data-toggle='tab' onclick=\"device_tab_onclick('sensors');\"><span class='badge bg-green'>"; echo $vilsdevices_sensors; echo "</span></a></li>";
                                //     echo "<li><a href=\"#loc_tab_loragateways\" data-toggle='tab' onclick=\"device_tab_onclick('loragateways');\"><span class='badge bg-purple-active'>"; echo $vilsdevices_loragateway; echo "</span></a></li>";
                                //     echo "<li><a href=\"#loc_tab_unknown_nfccards\" data-toggle='tab' onclick=\"device_tab_onclick('unknown_nfccards');\"><span class='badge bg-blue-active'>"; echo $vilsdevices_unknown_nfccard; echo "</span></a></li>";
                                // }
                                // else
                                // {
                                    echo "<li class='active'><a href=\"#loc_tab_tags\" data-toggle='tab' onclick=\"device_tab_onclick('tags');\"><span class='badge bg-blue-active'>"; echo $vilsdevices_tag; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_nfccards\" data-toggle='tab' onclick=\"device_tab_onclick('nfccards');\"><span class='badge bg-blue'>"; echo $vilsdevices_nfccard; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_uhfcards\" data-toggle='tab' onclick=\"device_tab_onclick('uhfcards');\"><span class='badge bg-teal-active'>"; echo $vilsdevices_uhfcard; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_anchors\" data-toggle='tab' onclick=\"device_tab_onclick('anchors');\"><span class='badge bg-green-active'>"; echo $vilsdevices_anchor; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_coords\" data-toggle='tab' onclick=\"device_tab_onclick('coords');\"><span class='badge bg-red-active'>"; echo $vilsdevices_coordinator; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_locators\" data-toggle='tab' onclick=\"device_tab_onclick('locators');\"><span class='badge bg-yellow-active'>"; echo $vilsdevices_locator; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_celllocators\" data-toggle='tab' onclick=\"device_tab_onclick('celllocators');\"><span class='badge bg-yellow-active'>"; echo $vilsdevices_celllocator; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_wifilocators\" data-toggle='tab' onclick=\"device_tab_onclick('wifilocators');\"><span class='badge bg-yellow-active'>"; echo $vilsdevices_wifilocator; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_edgecontrols\" data-toggle='tab' onclick=\"device_tab_onclick('edgecontrols');\"><span class='badge bg-yellow-active'>"; echo $vilsdevices_edgecontrol; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_nfclocators\" data-toggle='tab' onclick=\"device_tab_onclick('nfclocators');\"><span class='badge bg-yellow'>"; echo $vilsdevices_nfclocator; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_uhfreaders\" data-toggle='tab' onclick=\"device_tab_onclick('uhfreaders');\"><span class='badge bg-aqua-active'>"; echo $vilsdevices_uhfreader; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_sensors\" data-toggle='tab' onclick=\"device_tab_onclick('sensors');\"><span class='badge bg-green'>"; echo $vilsdevices_sensors; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_loragateways\" data-toggle='tab' onclick=\"device_tab_onclick('loragateways');\"><span class='badge bg-purple-active'>"; echo $vilsdevices_loragateway; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_unknown_nfccards\" data-toggle='tab' onclick=\"device_tab_onclick('unknown_nfccards');\"><span class='badge bg-blue-active'>"; echo $vilsdevices_unknown_nfccard; echo "</span></a></li>";
                                    echo "<li><a href=\"#loc_tab_unknown_uhfcards\" data-toggle='tab' onclick=\"device_tab_onclick('unknown_uhfcards');\"><span class='badge bg-teal-dsiabled'>"; echo $vilsdevices_unknown_uhfcard; echo "</span></a></li>";
                                // }
                            ?>
                        </ul>
                        <div class="tab-content">
                        <?php
                            // if ($_SESSION['type'] == 2 )
                            // {
                            //     echo "<div class='tab-pane        table-responsive' id='loc_tab_tags'>";
                            // }
                            // else
                            // {
                                echo "<div class='tab-pane active table-responsive' id='loc_tab_tags'>";
                            // }
                        ?>
                              <table class="table table-bordered">
                                  <thead>
                                  <tr style="background-color:#cce6ff;">
                                    <th style="width: 10px">#</th>
                                    <th><?php echo $vilsdevices_alive;?></th>
                                    <!-- <th><?php echo $vilsdevices_deviceid;?></th> -->
                                    <th><?php echo $vilsdevices_devicename;?></th>
                                    <th><?php echo $vilsdevices_typeindex;?></th>
                                    <?php if ($GL_CONFIG_SHOW_ROLE_INFO == 1)
                                        echo "<th>" . $vilsdevices_role . "</th>";
                                    ?>
                                    <!-- <th><?php echo $vilsdevices_resident;?></th> -->
                                    <th><?php echo $vilsdevices_user;?></th>
                                    <th><?php echo $vilsdevices_verifykey;?></th>
                                    <th><?php echo $vilsdevices_macaddress;?></th>
                                    <th><?php echo $vilsdevices_position;?></th>

                                    <?php if ($GL_CONFIG_SHOW_GPS_INFO == 1)
                                        echo "<th>" . $vilsdevices_GPS . "</th>";
                                    ?>

                                    <th><?php echo $vilsdevices_maplayer;?></th>
                                    <th><?php echo $vilsdevices_info;?></th>
                                    <th><?php echo $vilsdevices_available_anchors;?></th>
                                    <th style="display:none;" id='loc_tab_tag_prj'><?php echo $vilsdevices_projects;?></th>
                                  </tr>
                                  </thead>
                                  <tbody id="LOC_TAG_NODE_TABLE">
                                  </tbody>
                              </table>
                              <!-- <div class="btn-group">
                                  <button class="btn btn-block btn-danger btn-lg" id="deleteofflinetagbutton" style="display:none">Delete All Offline Tag</button>
                              </div> -->
                              <div class="btn-group">
                                 <?php
                                     //if ($_SESSION['type'] == 2)
                                     //{
                                         echo "<input type='text' id='tag_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                         echo '<a class="btn btn-success btn-xs" onclick="verify_new_device(\'tag\', \'tag_verifykey\',\'tag_verify_error\', \'tag_verify_pass\');"> '. $unlockpage_registertag .'</a>';
                                         echo "<div align='left'><h4><span class='label label-danger' id='tag_verify_error' style='disaply:none;'></span></h4></div>";
                                         echo "<div align='left'><h4><span class='label label-success' id='tag_verify_pass' style='disaply:none;'></span></h4></div>";
                                     //}
                                 ?>
                               </div>
                          </div>
                          <!-- /.tab-pane -->

                          <div class="tab-pane table-responsive" id="loc_tab_nfccards">

                              <div class="box">
                                  <div class="box-header with-border">
                                      <h3 class="box-title"></h3>
                                      <div class="box-tools pull-right">
                                          <ul class="pagination pagination-sm no-margin">
                                            <li><a href="#" onclick='nfccards_move_page_backward();return false;'>&laquo;</a></li>
                                            <li><a href="#" id="nfccards_id_current_page_num">1</a></li>
                                            <li><a href="#" onclick='nfccards_move_page_forward();return false;'>&raquo;</a></li>
                                          </ul>
                                        </ul>
                                      </div>
                                  </div>
                                  <!-- /.box-header -->
                                  <div class="box-body no-padding">

                                    <table class="table table-bordered">
                                      <thead>
                                        <tr style="background-color:#cce6ff;">
                                          <th style="width: 10px">#</th>
                                          <th style="width: 60px"><?php echo $vilsdevices_nfc_alive;?></th>
                                          <th><?php echo $vilsdevices_nfc_cardname;?></th>
                                          <th><?php echo $vilsdevices_nfc_cardtype;?></th>
                                          <th><?php echo $vilsdevices_nfc_id;?></th>
                                          <th><?php echo $vilsnfccard_user;?></th>
                                          <?php if ($_SESSION['groupid'] != 5 )
                                              echo "<th>" . $vilsdevices_nfc_maplayer . "</th>";
                                          ?>
                                          <!-- <th><?php echo $vilsdevices_nfc_maplayer;?></th> -->
                                          <th><?php echo $vilsnfccard_operation;?></th>
                                          <th style="display:none;" id='loc_tab_nfccard_prj'><?php echo $vilsdevices_projects;?></th>
                                        </tr>
                                      </thead>
                                      <tbody id="NFC_CARD_TABLE">
                                      </tbody>
                                    </table>
                                    <div class="btn-group">
                                        <?php
                                        // echo "<button class='btn btn-block btn-info btn-lg' id='scannewnfccardbutton' ><i class='fa fa-plus'></i>"; echo $vilsnfccard_scannewcard; echo "</button>";
                                        ?>
                                    </div>
                                </div>
                                <!-- /.box-body -->
                              </div>
                              <!-- /.box -->
                          </div>
                          <!-- /.tab-pane -->

                            <div class="tab-pane table-responsive" id="loc_tab_uhfcards">

                                <div class="box">
                                    <div class="box-header with-border">
                                        <h3 class="box-title"></h3>
                                        <div class="box-tools pull-right">
                                            <ul class="pagination pagination-sm no-margin">
                                              <li><a href="#" onclick='rfidcards_move_page_backward();return false;'>&laquo;</a></li>
                                              <li><a href="#" id="rfidcards_id_current_page_num">1</a></li>
                                              <li><a href="#" onclick='rfidcards_move_page_forward();return false;'>&raquo;</a></li>
                                            </ul>
                                          </ul>
                                        </div>
                                    </div>
                                    <!-- /.box-header -->
                                    <div class="box-body no-padding">
                                      <table class="table table-bordered">
                                        <thead>
                                          <tr style="background-color:#cce6ff;">
                                            <th style="width: 10px">#</th>
                                            <th style="width: 60px"><?php echo $vilsdevices_uhf_alive;?></th>
                                            <th><?php echo $vilsdevices_uhf_cardname;?></th>
                                            <th><?php echo $vilsdevices_uhf_cardtype;?></th>
                                            <th><?php echo $vilsdevices_uhf_id;?></th>
                                            <th><?php echo $vilsrfidcard_user;?></th>
                                            <th><?php echo $vilsdevices_uhf_maplayer;?></th>
                                            <th><?php echo $vilsrfidcard_operation;?></th>
                                            <th style="display:none;" id='loc_tab_uhfcard_prj'><?php echo $vilsdevices_projects;?></th>
                                          </tr>
                                        </thead>
                                        <tbody id="RFID_CARD_TABLE">
                                        </tbody>
                                      </table>
                                      <div class="btn-group">
                                          <?php
                                          // echo "<button class='btn btn-block btn-info btn-lg' id='scannewrfidcardbutton' ><i class='fa fa-plus'></i>"; echo $vilsrfidcard_scannewcard; echo "</button>";
                                          ?>
                                      </div>
                                  </div>
                                  <!-- /.box-body -->
                                </div>
                                <!-- /.box -->
                            </div>
                            <!-- /.tab-pane -->

                          <div class="tab-pane table-responsive" id="loc_tab_anchors">
                              <table class="table table-bordered">
                                  <thead>
                                  <tr style="background-color:#cce6ff;">
                                    <th style="width: 10px">#</th>
                                    <th><?php echo $vilsdevices_alive;?></th>
                                    <!-- <th><?php echo $vilsdevices_deviceid;?></th> -->
                                    <th><?php echo $vilsdevices_devicename;?></th>
                                    <th><?php echo $vilsdevices_typeindex;?></th>
                                    <th><?php echo $vilsdevices_verifykey;?></th>
                                    <th><?php echo $vilsdevices_macaddress;?></th>
                                    <th><?php echo $vilsdevices_position;?></th>
                                    <th><?php echo $vilsdevices_maplayer;?></th>
                                    <th><?php echo $vilsdevices_info;?></th>
                                    <th style="display:none;" id='loc_tab_anchor_prj'><?php echo $vilsdevices_projects;?></th>
                                  </tr>
                                  </thead>
                                  <tbody id="LOC_ANCHOR_NODE_TABLE">
                                  </tbody>
                              </table>
                              <!-- <div class="btn-group">
                                  <button class="btn btn-block btn-danger btn-lg" id="deleteofflineanchorbutton" style="display:none">Delete All Offline Anchor</button>
                              </div> -->
                              <div class="btn-group">
                                 <?php
                                     //if ($_SESSION['type'] == 2)
                                     //{
                                         echo "<input type='text' id='anchor_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                         echo '<a class="btn btn-success btn-xs" onclick="verify_new_device(\'anchor\', \'anchor_verifykey\',\'anchor_verify_error\', \'anchor_verify_pass\');"> '. $unlockpage_registeranchor .'</a>';
                                         echo "<div align='left'><h4><span class='label label-danger' id='anchor_verify_error' style='disaply:none;'></span></h4></div>";
                                         echo "<div align='left'><h4><span class='label label-success' id='anchor_verify_pass' style='disaply:none;'></span></h4></div>";
                                     //}
                                 ?>
                               </div>

                          </div>
                          <!-- /.tab-pane -->
                          <div class="tab-pane table-responsive" id="loc_tab_coords">
                              <table class="table table-bordered">
                                  <thead>
                                  <tr style="background-color:#cce6ff;">
                                    <th style="width: 10px">#</th>
                                    <th><?php echo $vilsdevices_alive;?></th>
                                    <th><?php echo $vilsdevices_ping;?></th>
                                    <!-- <th><?php echo $vilsdevices_deviceid;?></th> -->
                                    <th><?php echo $vilsdevices_devicename;?></th>
                                    <th><?php echo $vilsdevices_typeindex;?></th>
                                    <th><?php echo $vilsdevices_verifykey;?></th>
                                    <th><?php echo $vilsdevices_macaddress;?></th>
                                    <th><?php echo $vilsdevices_position;?></th>
                                    <th><?php echo $vilsdevices_maplayer;?></th>
                                    <th><?php echo $vilsdevices_info;?></th>
                                    <th><?php echo $vilsdevices_IP;?></th>
                                    <th><?php echo $vilsdevices_version;?></th>
                                    <th><?php echo $vilsdevices_CoordRunTime;?></th>
                                    <th><?php echo $vilsdevices_blacklist;?></th>
                                    <th><?php echo $vilsdevices_whitelist;?></th>
                                    <th><?php echo $vilsdevices_joinRangingRange;?></th>
                                    <th><?php echo $vilsdevices_joinNotRangingRange;?></th>
                                    <th><?php echo $vilsdevices_reset;?></th>
                                    <th style="display:none;" id='loc_tab_coord_prj'><?php echo $vilsdevices_projects;?></th>
                                  </tr>
                                  </thead>
                                  <tbody id="LOC_COORD_NODE_TABLE">
                                  </tbody>
                              </table>
                              <!-- <div class="btn-group">
                                  <button class="btn btn-block btn-danger btn-lg" id="deleteofflinecoordbutton" style="display:none">Delete All Offline Coordinator</button>
                              </div> -->
                              <div class="btn-group">
                                 <?php
                                     //if ($_SESSION['type'] == 2)
                                     //{
                                         echo "<input type='text' id='coord_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                         echo '<a class="btn btn-success btn-xs" onclick="verify_new_device(\'coord\', \'coord_verifykey\',\'coord_verify_error\', \'coord_verify_pass\');"> '. $unlockpage_registercoord .'</a>';
                                         echo "<div align='left'><h4><span class='label label-danger' id='coord_verify_error' style='disaply:none;'></span></h4></div>";
                                         echo "<div align='left'><h4><span class='label label-success' id='coord_verify_pass' style='disaply:none;'></span></h4></div>";
                                     //}
                                 ?>
                               </div>
                          </div>
                          <!-- /.tab-pane -->

                          <div class="tab-pane table-responsive" id="loc_tab_locators">
                              <table class="table table-bordered">
                                  <thead>
                                  <tr style="background-color:#cce6ff;">
                                    <th style="width: 10px">#</th>
                                    <th><?php echo $vilsdevices_alive;?></th>
                                    <!-- <th><?php echo $vilsdevices_deviceid;?></th> -->
                                    <th><?php echo $vilsdevices_devicename;?></th>
                                    <th><?php echo $vilsdevices_typeindex;?></th>
                                    <th><?php echo $vilsdevices_buttontype;?></th>
                                    <!-- <th><?php echo $vilsdevices_resident;?></th> -->
                                    <th><?php echo $vilsdevices_user;?></th>
                                    <th><?php echo $vilsdevices_verifykey;?></th>
                                    <th><?php echo $vilsdevices_macaddress;?></th>
                                    <th><?php echo $vilsdevices_LoRA;?></th>
                                    <th><?php echo $vilsdevices_position;?></th>
                                    <th><?php echo $vilsdevices_sensoralert;?></th>
                                    <th><?php echo $vilsdevices_maplayer;?></th>
                                    <th><?php echo $vilsdevices_info;?></th>
                                    <th style="display:none;" id='loc_tab_locator_prj'><?php echo $vilsdevices_projects;?></th>
                                  </tr>
                                  </thead>
                                  <tbody id="LOC_LOCATOR_NODE_TABLE">
                                  </tbody>
                              </table>
                              <div class="btn-group">
                                  <button class="btn btn-block btn-warning btn-lg" id="createnewlocatorbutton" style="display:none"><i class="fa fa-plus"></i>Create New Locator</button>
                                  <!-- <button class="btn btn-block btn-danger btn-lg" id="deleteofflinelocatorbutton" style="display:none">Delete All Offline Locator</button> -->
                              </div>
                              <div class="btn-group">
                                 <?php
                                     //if ($_SESSION['type'] == 2)
                                     //{
                                         echo "<input type='text' id='locator_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                         echo '<a class="btn btn-success btn-xs" onclick="verify_new_device(\'locator\', \'locator_verifykey\',\'locator_verify_error\', \'locator_verify_pass\');"> '. $unlockpage_registerlocator .'</a>';
                                         echo "<div align='left'><h4><span class='label label-danger' id='locator_verify_error' style='disaply:none;'></span></h4></div>";
                                         echo "<div align='left'><h4><span class='label label-success' id='locator_verify_pass' style='disaply:none;'></span></h4></div>";
                                     //}
                                 ?>
                               </div>
                           </div>
                          <!-- /.tab-pane -->

                          <div class="tab-pane table-responsive" id="loc_tab_celllocators">
                              <div class="box">
                                  <div class="box-header with-border">
                                      <h3 class="box-title"></h3>
                                      <div class="box-tools pull-right">
                                          <ul class="pagination pagination-sm no-margin">
                                            <li><a href="#" onclick='celllocators_move_page_backward();return false;'>&laquo;</a></li>
                                            <li><a href="#" id="celllocators_id_current_page_num">1</a></li>
                                            <li><a href="#" onclick='celllocators_move_page_forward();return false;'>&raquo;</a></li>
                                          </ul>
                                        </ul>
                                      </div>
                                  </div>
                                  <!-- /.box-header -->
                                  <div class="box-body no-padding">

                                      <table class="table table-bordered">
                                          <thead>
                                          <tr style="background-color:#cce6ff;">
                                              <th style="width: 10px">#</th>
                                              <th><?php echo $vilsdevices_alive;?></th>
                                              <th><?php echo $vilsdevices_devicename;?></th>
                                              <th><?php echo $vilsdevices_verifykey;?></th>
                                              <th><?php echo $vilsdevices_macaddress;?></th>
                                              <th><?php echo $vilsdevices_version;?></th>
                                              <th><?php echo $vilsdevices_info;?></th>
                                              <th style="display:none;" id='loc_tab_celllocator_prj'><?php echo $vilsdevices_projects;?></th>
                                          </tr>
                                        </thead>
                                        <tbody id="LOC_CELL_LOCATOR_NODE_TABLE">

                                        </tbody>

                                      </table>
                                      <div class="btn-group">
                                         <?php
                                             //if ($_SESSION['type'] == 2)
                                             //{
                                                 echo "<input type='text' id='celllocator_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                                 echo '<a class="btn btn-success btn-xs" onclick="verify_new_device(\'celllocator\', \'celllocator_verifykey\',\'celllocator_verify_error\', \'celllocator_verify_pass\');"> '. $unlockpage_registercelllocator .'</a>';
                                                 echo "<div align='left'><h4><span class='label label-danger' id='celllocator_verify_error' style='disaply:none;'></span></h4></div>";
                                                 echo "<div align='left'><h4><span class='label label-success' id='celllocator_verify_pass' style='disaply:none;'></span></h4></div>";
                                             //}
                                         ?>
                                       </div>
                                  </div>
                                  <!-- /.box-body -->
                                </div>
                                <!-- /.box -->
                          </div>
                          <!-- /.tab-pane -->

                        <div class="tab-pane table-responsive" id="loc_tab_wifilocators">
                            <div class="box">
                                <div class="box-header with-border">
                                    <h3 class="box-title"></h3>
                                    <div class="box-tools pull-right">
                                        <ul class="pagination pagination-sm no-margin">
                                          <li><a href="#" onclick='wifilocators_move_page_backward();return false;'>&laquo;</a></li>
                                          <li><a href="#" id="wifilocators_id_current_page_num">1</a></li>
                                          <li><a href="#" onclick='wifilocators_move_page_forward();return false;'>&raquo;</a></li>
                                        </ul>
                                      </ul>
                                    </div>
                                </div>
                                <!-- /.box-header -->
                                <div class="box-body no-padding">

                                    <table class="table table-bordered">
                                        <thead>
                                        <tr style="background-color:#cce6ff;">
                                          <th style="width: 10px">#</th>
                                          <th><?php echo $vilsdevices_wifilocator_alive;?></th>
                                          <th><?php echo $vilsdevices_wifilocator_name;?></th>
                                          <th><?php echo $vilsdevices_wifilocator_type;?></th>
                                          <th><?php echo $vilsdevices_verifykey;?></th>
                                          <th><?php echo $vilsdevices_wifilocator_macaddress;?></th>
                                          <th><?php echo $vilsdevices_wifilocator_apssid;?></th>
                                          <th><?php echo $vilsdevices_wifilocator_apmac;?></th>
                                          <th><?php echo $vilsdevices_version;?></th>
                                          <th><?php echo $vilsdevices_wifilocator_operation;?></th>
                                          <th style="display:none;" id='loc_tab_wifilocator_prj'><?php echo $vilsdevices_projects;?></th>
                                        </tr>
                                      </thead>
                                      <tbody id="LOC_WIFI_LOCATOR_NODE_TABLE">

                                      </tbody>

                                    </table>
                                    <div class="btn-group">
                                       <?php
                                           //if ($_SESSION['type'] == 2)
                                           //{
                                               echo "<input type='text' id='wifilocator_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                               echo '<a class="btn btn-success btn-xs" onclick="verify_new_device(\'wifilocator\', \'wifilocator_verifykey\',\'wifilocator_verify_error\', \'wifilocator_verify_pass\');"> '. $unlockpage_registerwifilocator .'</a>';
                                               echo "<div align='left'><h4><span class='label label-danger' id='wifilocator_verify_error' style='disaply:none;'></span></h4></div>";
                                               echo "<div align='left'><h4><span class='label label-success' id='wifilocator_verify_pass' style='disaply:none;'></span></h4></div>";
                                           //}
                                       ?>
                                     </div>
                                </div>
                                <!-- /.box-body -->
                              </div>
                              <!-- /.box -->
                        </div>
                        <!-- /.tab-pane -->

                        <div class="tab-pane table-responsive" id="loc_tab_edgecontrols">
                            <div class="box">
                                <div class="box-header with-border">
                                    <h3 class="box-title"></h3>
                                    <div class="box-tools pull-right">
                                        <ul class="pagination pagination-sm no-margin">
                                          <li><a href="#" onclick='edgecontrols_move_page_backward();return false;'>&laquo;</a></li>
                                          <li><a href="#" id="edgecontrols_id_current_page_num">1</a></li>
                                          <li><a href="#" onclick='edgecontrols_move_page_forward();return false;'>&raquo;</a></li>
                                        </ul>
                                      </ul>
                                    </div>
                                </div>
                                <!-- /.box-header -->
                                <div class="box-body no-padding">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr style="background-color:#cce6ff;">
                                          <th style="width: 10px">#</th>
                                          <th><?php echo $vilsdevices_edgecontol_alive;?></th>
                                          <th><?php echo $vilsdevices_edgecontol_name;?></th>
                                          <th><?php echo $vilsdevices_edgecontol_type;?></th>
                                          <th><?php echo $vilsdevices_verifykey;?></th>
                                          <th><?php echo $vilsdevices_edgecontol_macaddress;?></th>
                                          <th><?php echo $vilsdevices_edgecontol_LoRAMacaddress;?></th>
                                          <th><?php echo $vilsdevices_edgecontol_gwid;?></th>
                                          <th><?php echo $vilsdevices_edgecontol_ip;?></th>
                                          <th><?php echo $vilsdevices_edgecontol_version;?></th>
                                          <th><?php echo $vilsdevices_wifilocator_operation;?></th>
                                          <th style="display:none;" id='loc_tab_edgecontol_prj'><?php echo $vilsdevices_projects;?></th>
                                        </tr>
                                      </thead>
                                      <tbody id="LOC_EDGE_CONTROL_NODE_TABLE">

                                      </tbody>

                                    </table>
                                    <div class="btn-group">
                                       <?php
                                           //if ($_SESSION['type'] == 2)
                                           //{
                                               echo "<input type='text' id='edgecontol_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                               echo '<a class="btn btn-success btn-xs" onclick="verify_new_device(\'edgecontol\', \'edgecontol_verifykey\',\'edgecontol_verify_error\', \'edgecontol_verify_pass\');"> '. $unlockpage_registeredgecontol .'</a>';
                                               echo "<div align='left'><h4><span class='label label-danger' id='edgecontol_verify_error' style='disaply:none;'></span></h4></div>";
                                               echo "<div align='left'><h4><span class='label label-success' id='edgecontol_verify_pass' style='disaply:none;'></span></h4></div>";
                                           //}
                                       ?>
                                     </div>
                                </div>
                                <!-- /.box-body -->
                              </div>
                              <!-- /.box -->
                        </div>
                        <!-- /.tab-pane -->

                        <?php
                            // if ($_SESSION['type'] == 2 )
                            // {
                            //     echo "<div class='tab-pane active table-responsive' id='loc_tab_nfclocators'>";
                            // }
                            // else
                            // {
                                echo "<div class='tab-pane table-responsive' id='loc_tab_nfclocators'>";
                            // }
                        ?>

                        <div class="box">
                            <div class="box-header with-border">
                                <h3 class="box-title"></h3>
                                <div class="box-tools pull-right">
                                    <ul class="pagination pagination-sm no-margin">
                                      <li><a href="#" onclick='nfclocators_move_page_backward();return false;'>&laquo;</a></li>
                                      <li><a href="#" id="nfclocators_id_current_page_num">1</a></li>
                                      <li><a href="#" onclick='nfclocators_move_page_forward();return false;'>&raquo;</a></li>
                                    </ul>
                                  </ul>
                                </div>
                            </div>
                            <!-- /.box-header -->
                            <div class="box-body no-padding">

                            <table class="table table-bordered">
                              <thead>
                                <tr style="background-color:#cce6ff;">
                                  <th style="width: 10px">#</th>
                                  <th><?php echo $vilsdevices_nfclocator_alive;?></th>
                                  <th><?php echo $vilsdevices_nfclocator_name;?></th>
                                  <th><?php echo $vilsdevices_nfclocator_type;?></th>
                                  <th><?php echo $vilsdevices_verifykey;?></th>
                                  <th><?php echo $vilsdevices_nfclocator_macaddress;?></th>
                                  <th><?php echo $vilsdevices_nfclocator_LoRAMacaddress;?></th>
                                  <th><?php echo $vilsdevices_nfclocator_gwid;?></th>
                                  <th><?php echo $vilsdevices_version;?></th>
                                  <th><?php echo $vilsdevices_nfclocator_operation;?></th>
                                  <th style="display:none;" id='loc_tab_nfclocator_prj'><?php echo $vilsdevices_projects;?></th>
                                </tr>
                              </thead>
                              <tbody id="LOC_NFCLOCATOR_TABLE">
                              </tbody>
                             </table>

                             <div class="btn-group">
                                <?php
                                    //if ($_SESSION['type'] == 2 )
                                    //{
                                        echo "<input type='text' id='nfclocator_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                        echo "<a class='btn btn-success btn-xs' onclick='verify_new_nfclocator();'>".$unlockpage_registerdevice."</a>";
                                        echo "<div align='left'><h4><span class='label label-danger' id='nfclocator_verify_error' style='disaply:none;'></span></h4></div>";
                                        echo "<div align='left'><h4><span class='label label-success' id='nfclocator_verify_pass' style='disaply:none;'></span></h4></div>";
                                    //}

                                    if ($_SESSION['type'] != 2 )
                                    {
                                        echo "<button class='btn btn-block btn-warning btn-lg' id='createnewnfclocatorbutton' ><i class='fa fa-plus'></i>"; echo $vilsdevices_nfclocator_creatnewdevice; echo "</button>";
                                    }
                                ?>
                              </div>
                            </div>
                            <!-- /.box-body -->
                          </div>
                          <!-- /.box -->
                        </div>
                        <!-- /.tab-pane -->

                        <div class="tab-pane table-responsive" id="loc_tab_uhfreaders">

                            <div class="box">
                                <div class="box-body no-padding">
                                  <table class="table table-bordered">
                                    <thead>
                                      <tr style="background-color:#cce6ff;">
                                        <th style="width: 10px">#</th>
                                        <th><?php echo $vilsrfiddevice_alive;?></th>
                                        <th><?php echo $vilsrfiddevice_name;?></th>
                                        <th><?php echo $vilsrfiddevice_type;?></th>
                                        <th><?php echo $vilsrfiddevice_ip;?></th>
                                        <th><?php echo $vilsdevices_verifykey;?></th>
                                        <th><?php echo $vilsrfiddevice_macaddress;?></th>
                                        <th><?php echo $vilsrfiddevice_LoRAMacaddress;?></th>
                                        <th><?php echo $vilsrfiddevice_gwid;?></th>
                                        <th><?php echo $vilsrfiddevice_operation;?></th>
                                        <th style="display:none;" id='loc_tab_uhfreader_prj'><?php echo $vilsdevices_projects;?></th>
                                      </tr>
                                    </thead>
                                    <tbody id="RFID_DEVICE_TABLE">
                                    </tbody>
                                  </table>
                                  <div class="btn-group">
                                      <?php
                                          echo "<button class='btn btn-block btn-info btn-lg' id='createnewrfiddevicebutton' ><i class='fa fa-plus'></i>"; echo $vilsrfiddevice_creatnewdevice; echo "</button>";
                                      ?>
                                  </div>
                                  <div class="btn-group">
                                     <?php
                                         //if ($_SESSION['type'] == 2)
                                         //{
                                             echo "<input type='text' id='uhfreader_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                             echo '<a class="btn btn-success btn-xs" onclick="verify_new_device(\'uhfreader\', \'uhfreader_verifykey\',\'uhfreader_verify_error\', \'uhfreader_verify_pass\');"> '. $unlockpage_registeruhfreader .'</a>';
                                             echo "<div align='left'><h4><span class='label label-danger' id='uhfreader_verify_error' style='disaply:none;'></span></h4></div>";
                                             echo "<div align='left'><h4><span class='label label-success' id='uhfreader_verify_pass' style='disaply:none;'></span></h4></div>";
                                         //}
                                     ?>
                                   </div>
                              </div>
                              <!-- /.box-body -->
                            </div>
                            <!-- /.box -->

                        </div>
                        <!-- /.tab-pane -->

                        <div class="tab-pane table-responsive" id="loc_tab_sensors">

                            <div class="box">

                                <div class="box-header with-border">
                                    <h3 class="box-title"></h3>
                                    <div class="box-tools pull-right">
                                        <ul class="pagination pagination-sm no-margin">
                                          <li><a href="#" onclick='sensors_move_page_backward();return false;'>&laquo;</a></li>
                                          <li><a href="#" id="sensors_id_current_page_num">1</a></li>
                                          <li><a href="#" onclick='sensors_move_page_forward();return false;'>&raquo;</a></li>
                                        </ul>
                                      </ul>
                                    </div>
                                </div>
                                <!-- /.box-header -->

                                <div class="box-body no-padding">
                                  <table class="table table-bordered">
                                    <thead>
                                      <tr style="background-color:#cce6ff;">
                                        <th style="width: 10px">#</th>
                                        <th><?php echo $vilssensordevice_alive;?></th>
                                        <th><?php echo $vilssensordevice_id;?></th>
                                        <th><?php echo $vilssensordevice_name;?></th>
                                        <th><?php echo $vilssensordevice_type;?></th>
                                        <th><?php echo $vilsdevices_verifykey;?></th>
                                        <th><?php echo $vilssensordevice_LngLat;?></th>
                                        <th><?php echo $vilssensordevice_Battery;?></th>
                                        <th><?php echo $vilssensordevice_info;?></th>
                                        <th><?php echo $vilssensordevice_version;?></th>
                                        <th><?php echo $vilssensordevice_operation;?></th>
                                      </tr>
                                    </thead>
                                    <tbody id="SENSOR_DEVICE_TABLE">
                                    </tbody>
                                  </table>
                                  <div class="btn-group">
                                     <?php
                                         //if ($_SESSION['type'] == 2)
                                         //{
                                             echo "<input type='text' id='sensor_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                             echo "<a class='btn btn-success btn-xs' onclick='verify_new_sensor();'>".$unlockpage_registersensordevice."</a>";
                                             echo "<div align='left'><h4><span class='label label-danger' id='sensor_verify_error' style='disaply:none;'></span></h4></div>";
                                             echo "<div align='left'><h4><span class='label label-success' id='sensor_verify_pass' style='disaply:none;'></span></h4></div>";
                                         //}
                                     ?>
                                   </div>
                              </div>
                              <!-- /.box-body -->
                            </div>
                            <!-- /.box -->

                        </div>
                        <!-- /.tab-pane -->


                          <div class="tab-pane table-responsive" id="loc_tab_loragateways">
                              <table class="table table-bordered">
                                  <thead>
                                  <tr style="background-color:#cce6ff;">
                                    <th style="width: 10px">#</th>
                                    <th><?php echo $vilsdevices_alive;?></th>
                                    <th><?php echo $vilsdevices_ping;?></th>
                                    <th><?php echo $vilsdevices_MQTT;?></th>
                                    <th><?php echo $vilsdevices_verifykey;?></th>
                                    <th><?php echo $vilsdevices_gwid;?></th>
                                    <th><?php echo $vilsdevices_IP;?></th>
                                    <th><?php echo $vilsdevices_position;?></th>
                                    <th><?php echo $vilsdevices_maplayer;?></th>
                                    <th><?php echo $vilsdevices_sf;?></th>
                                    <th><?php echo $vilsdevices_repeater;?></th>
                                    <th><?php echo $vilsdevices_channel;?></th>
                                    <th><?php echo $vilsdevices_info;?></th>
                                    <th style="display:none;" id='loc_tab_loragateway_prj'><?php echo $vilsdevices_projects;?></th>
                                  </tr>
                                  </thead>
                                  <tbody id="LOC_LORAGATEWAY_NODE_TABLE">
                                  </tbody>
                              </table>
                              <div class="btn-group">
                                 <?php
                                     //if ($_SESSION['type'] == 2)
                                     //{
                                         echo "<input type='text' id='loragateway_verifykey' placeholder='".$unlockpage_verifykey."' />";
                                         echo '<a class="btn btn-success btn-xs" onclick="verify_new_device(\'loragateway\', \'loragateway_verifykey\',\'loragateway_verify_error\', \'loragateway_verify_pass\');"> '. $unlockpage_registerloragateway .'</a>';
                                         echo "<div align='left'><h4><span class='label label-danger' id='loragateway_verify_error' style='disaply:none;'></span></h4></div>";
                                         echo "<div align='left'><h4><span class='label label-success' id='loragateway_verify_pass' style='disaply:none;'></span></h4></div>";
                                     //}
                                 ?>
                               </div>
                         </div>
                          <!-- /.tab-pane -->

                          <div class="tab-pane table-responsive" id="loc_tab_unknown_nfccards">

                              <div class="box">
                                  <div class="box-header with-border">
                                      <h3 class="box-title"></h3>

                                      <span>&nbsp;</span>
                                      <a class="btn btn-success btn-xs" onclick="create_unknown_nfccards_xlsx_report();">產生XLSX檔案</a>
                                      <span id="unknown_nfccards_xlsxreport_status"></span>

                                      <div class="box-tools pull-right">
                                          <ul class="pagination pagination-sm no-margin">
                                            <li><a href="#" onclick='unknown_nfccards_move_page_backward();return false;'>&laquo;</a></li>
                                            <li><a href="#" id="unknown_nfccards_id_current_page_num">1</a></li>
                                            <li><a href="#" onclick='unknown_nfccards_move_page_forward();return false;'>&raquo;</a></li>
                                          </ul>
                                        </ul>
                                      </div>
                                  </div>
                                  <!-- /.box-header -->
                                  <div class="box-body no-padding">

                                    <table class="table table-bordered">
                                      <thead>
                                        <tr style="background-color:#cce6ff;">
                                          <th style="width: 10px">#</th>
                                          <th style="width: 60px"><?php echo $vilsnfccard_unknown_alive;?></th>
                                          <th><?php echo $vilsnfccard_unknown_id;?></th>
                                          <th><?php echo $vilsnfccard_unknown_nfcdevice;?></th>
                                          <th><?php echo $vilsnfccard_unknown_updatetime;?></th>
                                          <th><?php echo $vilsnfccard_unknown_operation;?></th>
                                          <th style="display:none;" id='loc_tab_unknown_nfccard_prj'><?php echo $vilsdevices_projects;?></th>
                                        </tr>
                                      </thead>
                                      <tbody id="UNKNOWN_NFC_CARD_TABLE">
                                      </tbody>
                                    </table>
                                    <!-- <div class="btn-group">
                                        <button class="btn btn-block btn-danger btn-lg" id="deleteunknownnfccardsbutton">刪除所有未註冊NFC卡片</button>
                                    </div> -->
                                </div>
                                <!-- /.box-body -->
                              </div>
                              <!-- /.box -->
                          </div>
                          <!-- /.tab-pane -->


                          <div class="tab-pane table-responsive" id="loc_tab_unknown_uhfcards">

                              <div class="box">
                                  <div class="box-header with-border">
                                      <h3 class="box-title"></h3>

                                        <span>&nbsp;</span>
                                        <a class="btn btn-success btn-xs" onclick="create_unknown_uhfcards_xlsx_report();">產生XLSX檔案</a>
                                        <span id="unknown_uhfcards_xlsxreport_status"></span>

                                      <div class="box-tools pull-right">
                                          <ul class="pagination pagination-sm no-margin">
                                            <li><a href="#" onclick='unknown_rfidcards_move_page_backward();return false;'>&laquo;</a></li>
                                            <li><a href="#" id="unknown_rfidcards_id_current_page_num">1</a></li>
                                            <li><a href="#" onclick='unknown_rfidcards_move_page_forward();return false;'>&raquo;</a></li>
                                          </ul>
                                        </ul>
                                      </div>
                                  </div>
                                  <!-- /.box-header -->
                                  <div class="box-body no-padding">
                                    <table class="table table-bordered">
                                      <thead>
                                        <tr style="background-color:#cce6ff;">
                                          <th style="width: 10px">#</th>
                                          <th style="width: 60px"><?php echo $vilsrfidcard_unknown_alive;?></th>
                                          <th><?php echo $vilsrfidcard_unknown_id;?></th>
                                          <th><?php echo $vilsrfidcard_unknown_nfcdevice;?></th>
                                          <th><?php echo $vilsrfidcard_unknown_updatetime;?></th>
                                          <th><?php echo $vilsrfidcard_unknown_operation;?></th>
                                          <th style="display:none;" id='loc_tab_unknown_uhfcard_prj'><?php echo $vilsdevices_projects;?></th>
                                        </tr>
                                      </thead>
                                      <tbody id="UNKNOWN_RFID_CARD_TABLE">
                                      </tbody>
                                    </table>
                                    <!-- <div class="btn-group">
                                        <button class="btn btn-block btn-danger btn-lg" id="deleteunknownrfidcardsbutton">刪除所有未註冊UHF卡片</button>
                                    </div> -->
                                </div>
                                <!-- /.box-body -->
                              </div>
                              <!-- /.box -->
                          </div>
                          <!-- /.tab-pane -->



                        </div>
                        <!-- /.tab-content -->
                      </div>
                      <!-- nav-tabs-custom -->
                      <!-- <div id="LOC_NODE_TABLE_MSG"><?php echo $vilsdevices_loading;?></div> -->
                  </div>
                  <!-- /.box-body -->
                </div>
                <!-- /.box -->



                <!-- Control Sidebar -->
                <aside class="control-sidebar control-sidebar-dark">
                  <!-- Create the tabs -->
                  <ul class="nav nav-tabs nav-justified control-sidebar-tabs">
                   <li><a href="#control-sidebar-settings-tab" data-toggle="control-sidebar"><i class="fa fa-gears"></i></a></li>
                  </ul>

                  <!-- Tab panes -->
                  <div class="tab-content">

                    <!-- Settings tab content -->
                    <div class="tab-pane active" id="control-sidebar-settings-tab">
                      <h3 class="control-sidebar-heading"><?php echo $vilsdevices_general;?></h3>
                      <div class="form-group">
                        <label class="control-sidebar-subheading"><?php echo $vilsdevices_editinfo;?>
                          <button class="btn btn-primary pull-right" id="toggleeditnodebtn"><?php echo $vilsdevices_toggle;?></button>
                        </label>
                      </div>
                    </div>
                    <!-- /.tab-pane -->
                   </div>
                    <!-- /.tab-content -->
                </aside>
                <!-- /.control-sidebar -->
                <!-- Add the sidebar's background. This div must be placed
                     immediately after the control sidebar -->
                <div class="control-sidebar-bg"></div>

    <script>
      var project_id = <?php echo $project_id ?>;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var groupid = "<?php echo $_SESSION["groupid"] ?>";
      var GL_CONFIG_SHOW_ROLE_INFO = <?php echo $GL_CONFIG_SHOW_ROLE_INFO ?>;
      var GL_CONFIG_SHOW_GPS_INFO = <?php echo $GL_CONFIG_SHOW_GPS_INFO ?>;

      <?php
          // if ($_SESSION['type'] == 2 )
          // {
          //     echo "var gl_activetab = 'activetab_nfclocators';";
          // }
          // else
          // {
             echo " var gl_activetab = 'activetab_tags';";
          // }
      ?>

    </script>

  <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <!-- <script src="/js/nodelist.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

  <script src="/js/nodelist_src/00-nodelist.js"></script>
  <script src="/js/nodelist_src/01-nfclocator.js"></script>
  <script src="/js/nodelist_src/02-nfccards.js"></script>
  <script src="/js/nodelist_src/03-unknownnfccards.js"></script>
  <script src="/js/nodelist_src/04-wifilocator.js"></script>
  <script src="/js/nodelist_src/05-celllocator.js"></script>
  <script src="/js/nodelist_src/06-edgecontrol.js"></script>
  <script src="/js/nodelist_src/07-sensors.js"></script>
  <script src="/js/rfiddevices_src/00-rfiddevices.js"></script>
  <script src="/js/rfidcards_src/00-rfidcards.js"></script>
  <script src="/js/rfidcards_src/01-unknownrfidcards.js"></script>

  <script>
  function toggleOn()
  {
      $('#changemap_aside_apnel').css('display','block');
  }
  function toggleOff()
  {
      $('#changemap_aside_apnel').css('display','none');
  }
  </script>
</html>
