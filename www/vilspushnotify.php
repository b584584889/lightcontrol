<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link rel="shortcut icon" sizes="16x16 32x32 48x48 64x64" href="/images/favicon.ico">
    <link href="/css/vils.min.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Push Notification</title>
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


      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilspushnotify_title;?></h3>
          <div class="box-tools pull-right">
              <button type="button" class="btn btn-box-tool" onclick="open_pushnotify_page();return false;"><i class="fa fa-external-link"></i>
              </button>
          </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">
    		<div class="col-xs-12 col-sm-3 col-md-3 col-lg-3">
    			<div class="box">
    				<div class="box-body no-padding">
                        <div class="nav-tabs-custom">
                          <ul class="nav nav-tabs">
                            <li class="active"><a href="#loc_tab_tags" data-toggle="tab"><span class="badge bg-blue-active"><?php echo $vilsdevices_tag;?></span></a></li>
                            <li><a href="#loc_tab_locators" data-toggle="tab"><span class="badge bg-yellow-active"><?php echo $vilsdevices_locator;?></span></a></li>
                            <li><a href="#loc_tab_employees" data-toggle="tab"><span class="badge bg-purple"><?php echo $vilspushnotify_employees;?></span></a></li>
                            <li><a href="#loc_tab_search" data-toggle="tab"><button type="button" class="btn bg-orange btn-flat"><?php echo $vilspushnotify_search;?></button></a></li>
                            <li><a href="#loc_tab_setting" data-toggle="tab"><button type="button" class="btn bg-maroon btn-flat"><?php echo $vilspushnotify_setting;?></button></a></li>
                          </ul>

                          <div class="tab-content">

                            <div class="tab-pane active table-responsive" id="loc_tab_tags">
                                <div style="height:650px;overflow:scroll">
            					<table class="table table-hover">
            						<thead>
            							<tr style="background-color:#FFFFFF;">
            								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
            								<th style="width:60px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdevices_devicename;?></th>
                                            <th style="width:60px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdevices_role;?></th>
            							</tr>
            						</thead>
            						<tbody id="tag_list_content">

            						</tbody>
            					</table>
                                </div>
                            </div>
                             <!-- /.tab-pane -->

                             <div class="tab-pane table-responsive" id="loc_tab_locators">
                                 <div style="height:650px;overflow:scroll">
             					<table class="table table-hover">
             						<thead>
             							<tr style="background-color:#FFFFFF;">
             								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
             								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdevices_devicename;?></th>
                                            <th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdevices_buttontype;?></th>
             							</tr>
             						</thead>
             						<tbody id="locator_list_content">

             						</tbody>
             					</table>
                                 </div>
                             </div>
                              <!-- /.tab-pane -->

                               <div class="tab-pane table-responsive" id="loc_tab_employees">
                                   <div style="height:650px;overflow:scroll">
                       					<table class="table table-hover">
                       						<thead>
                       							<tr style="background-color:#FFFFFF;">
                       								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                       								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdevices_devicename;?></th>
                       							</tr>
                       						</thead>
                       						<tbody id="employee_list_content">

                       						</tbody>
                       					</table>
                                   </div>
                               </div>
                                <!-- /.tab-pane -->

                                <div class="tab-pane table-responsive" id="loc_tab_search">
               					<table class="table table-hover">
               						<thead>
                                           <input type="text" id="loc_tab_search_text">
               						</thead>
               						<tbody id="search_list_content">

               						</tbody>
               					</table>
                                </div>
                                <!-- /.tab-pane -->


                                <div class="tab-pane table-responsive" id="loc_tab_setting">
                                    <div style="height:650px;overflow:scroll">

                                        <div class="box">
                                            <div class="box-body no-padding">
                                                <div class="form-group">
                                                  <label>Push Notification Prefix</label>
                                                  <input type="text" class="form-control" id="PushNotifyPrefixInputText" value="">
                                                  <label>第一次訂閱推播歡迎標題</label>
                                                  <input type="text" class="form-control" id="PushNotifyWelcomeTitleInputText" value="歡迎使用">
                                                  <label>第一次訂閱推播歡迎內容</label>
                                                  <input type="text" class="form-control" id="PushNotifyWelcomeBodyInputText" value="感謝您使用北瀚雲端推播系統">
                                                  <div class="checkbox">
                                                    <label>
                                                      <input type="checkbox" id="EnablePushNotifyCheckBtn"> Enable Push Notification
                                                    </label>
                                                  </div>
                                                </div>
                                            </div>
                                            <!-- /.box-body -->
                                            <div class="box-footer">
                                                <div class="callout callout-warning" id="pushnotifyconfig_warning_callout" style="display:none">
                                                  <p>Updating...</p>
                                                </div>
                                                <div class="callout callout-success" id="pushnotifyconfigupdate_callout" style="display:none">
                                                  <p>Update Successfully!</p>
                                                </div>
                                                <button type="submit" class="btn btn-primary pull-right" id="pushnotifyconfigupdatebtn">Update</button>
                                            </div>
                                            <!-- /.box-footer -->
                                          </div>
                                          <!-- /.box -->

                                    </div>
                                </div>
                                 <!-- /.tab-pane -->

                           </div>
                           <!-- /.tab-content -->
                        </div>
                        <!-- nav-tabs-custom -->

    				</div>
    				<!-- /.box-body -->
    			</div>
    			<!-- /.box -->
    		</div>
    		<!-- col  -->

    		<div class="col-xs-12 col-sm-9 col-md-9 col-lg-9">

    			<div class="box">
    				<div class="box-body no-padding">

    					<div id="record_box_content" style="background-color:rgb(236, 240, 245);">

                            <section class="content-header">
                              <h1>
                                <?php echo $vilspushnotify_target ?>
                                <span class="badge bg-green" id="record_box_title">Tag</span>
                              </h1>
                            </section>

                            <!-- Main content -->
                            <section class="content">

                              <!-- row -->
                              <div class="row">
                                <div class="col-md-12">

                                    <table class="table table-bordered" id="PUSH_NOTIFY_TABLE">
                                        <thead>
                                          <tr>
                                            <th style="width: 150px"></th>
                                            <th></th>
                                            <th style="width: 20px"></th>
                                          </tr>
                                        </thead>
                                      <tbody id="PUSH_NOTIFY_TABLE_BODY">

                                      </tbody>
                                    </table>
                                  </ul>
                                  <!-- /.ul timeline-->
                                </div>
                                <!-- /.col -->
                              </div>
                              <!-- /.row -->

                            </section>
                            <!-- /.content -->

    					</div>
    					<!-- /.record_box_content -->
    				</div>
    				<!-- /.box-body -->
    			</div>
    			<!-- box  -->
    		</div>
    		<!-- /.col -->
    	</div>
    	<!-- /.box-body no-padding -->

      </div>
      <!-- /.box -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/pushnotify.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/pushnotify_src/00-pushnotify.js"></script> -->
</html>
