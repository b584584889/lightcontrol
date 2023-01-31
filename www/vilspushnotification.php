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
    <title>SMIMS VILS IoT - Push Notification</title>

    <style>
        .ui-datetimepicker { width: 10em; }
    </style>

  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilspushnotification_title;?></h3>
            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_pushnotification_page();return false;"><i class="fa fa-external-link"></i>
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
                               <?php
                                if($GL_CONFIG_ENABLE_SEND_ALL_PUSHNOTIFICATION)
                                {
                                    echo "<li class='active'><a href='#loc_tab_tags' data-toggle='tab'><span class='badge bg-blue-active'>"; echo $vilspushnotification_tag; echo "</span></a></li>";
                                    echo "<li><a href='#loc_tab_locators' data-toggle='tab'><span class='badge bg-yellow-active'>"; echo $vilspushnotification_locator; echo "</span></a></li>";
                                    echo "<li><a href='#loc_tab_roles' data-toggle='tab'><span class='badge bg-purple-active'>"; echo $vilspushnotification_role; echo "</span></a></li>";
                                    echo "<li><a href='#loc_tab_subgroup' data-toggle='tab'><span class='badge bg-green'>"; echo $vilspushnotification_subgroup; echo "</span></a></li>";
                                    echo "<li><a href='#loc_tab_user' data-toggle='tab'><span class='badge bg-aqua-active'>"; echo $vilspushnotification_user; echo "</span></a></li>";
                                }
                                else
                                {
                                    echo "<li class='active'><a href='#loc_tab_user' data-toggle='tab'><span class='badge bg-blue-active'>"; echo $vilspushnotification_user; echo "</span></a></li>";
                                    echo "<li><a href='#loc_tab_subgroup' data-toggle='tab'><span class='badge bg-green'>"; echo $vilspushnotification_subgroup; echo "</span></a></li>";
                                }
                               ?>

                                <?php
                                 if($GL_CONFIG_ENABLE_SEND_ALL_PUSHNOTIFICATION)
                                 {
                                    echo "<li><a href='#loc_tab_search' data-toggle='tab'>搜尋</a></li>";
                                 }
                                ?>
                              </ul>

                              <div class="tab-content">
                                <?php if ( $GL_CONFIG_ENABLE_SEND_ALL_PUSHNOTIFICATION ): ?>

                                <div class="tab-pane active table-responsive" id="loc_tab_tags">
                                    <div style="height:650px;overflow:scroll">
                					<table class="table table-hover">
                						<thead>
                							<tr style="background-color:#FFFFFF;">
                								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                                <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><input type="checkbox" id="tag_list_checkbox" onclick="tag_list_checkbox_click();"/></th>
                								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdevices_devicename;?></th>
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
                                                <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><input type="checkbox" id="locator_list_checkbox" onclick="locator_list_checkbox_click();"/></th>
                								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsdevices_devicename;?></th>
                 							</tr>
                 						</thead>
                 						<tbody id="locator_list_content">

                 						</tbody>
                 					</table>
                                     </div>
                                 </div>
                                  <!-- /.tab-pane -->

                               <div class="tab-pane table-responsive" id="loc_tab_roles">
                                   <div style="height:650px;overflow:scroll">
               					<table class="table table-hover">
               						<thead>
               							<tr style="background-color:#FFFFFF;">
               								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                              <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><input type="checkbox" id="role_list_checkbox" onclick="role_list_checkbox_click();"/></th>
              								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspushnotification_role;?></th>
               							</tr>
               						</thead>
               						<tbody id="role_list_content">

               						</tbody>
               					</table>
                                   </div>
                               </div>
                                <!-- /.tab-pane -->

                               <div class="tab-pane table-responsive" id="loc_tab_subgroup">
                                   <div style="height:650px;overflow:scroll">
               					<table class="table table-hover">
               						<thead>
               							<tr style="background-color:#FFFFFF;">
               								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                              <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><input type="checkbox" id="subgroup_list_checkbox" onclick="subgroup_list_checkbox_click();"/></th>
              								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspushnotification_subgroup;?></th>
               							</tr>
               						</thead>
               						<tbody id="subgroup_list_content">

               						</tbody>
               					</table>
                                   </div>
                               </div>
                                <!-- /.tab-pane -->

                                <div class="tab-pane active table-responsive" id="loc_tab_user">
                                    <div style="height:650px;overflow:scroll">
                					<table class="table table-hover">
                						<thead>
                							<tr style="background-color:#FFFFFF;">
                								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                               <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><input type="checkbox" id="user_list_checkbox" onclick="user_list_checkbox_click();"/></th>
               								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspushnotification_username;?></th>
                                            <th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspushnotification_subgroup;?></th>
                							</tr>
                						</thead>
                						<tbody id="user_list_content">

                						</tbody>
                					</table>
                                    </div>
                                </div>
                                 <!-- /.tab-pane -->

                                <div class="tab-pane table-responsive" id="loc_tab_search">
                                   <table class="table table-hover">
                                       <thead>
                                           <input type="text" id="loc_tab_search_text">
                                           <tr style="background-color:#FFFFFF;">
                  								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                                 <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"></th>
                 								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspushnotification_searchresultname;?></th>
                  							</tr>
                                       </thead>
                                       <tbody id="search_list_content">

                                       </tbody>
                                   </table>
                                </div>
                                <!-- /.tab-pane -->

                                <?php else: ?>

                                <div class="tab-pane active table-responsive" id="loc_tab_user">
                                    <div style="height:650px;overflow:scroll">
                					<table class="table table-hover">
                						<thead>
                							<tr style="background-color:#FFFFFF;">
                								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                               <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><input type="checkbox" id="user_list_checkbox" onclick="user_list_checkbox_click();"/></th>
               								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspushnotification_username;?></th>
                                            <th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspushnotification_subgroup;?></th>
                							</tr>
                						</thead>
                						<tbody id="user_list_content">

                						</tbody>
                					</table>
                                    </div>
                                </div>
                                 <!-- /.tab-pane -->

                                 <div class="tab-pane table-responsive" id="loc_tab_subgroup">
                                     <div style="height:650px;overflow:scroll">
                 					<table class="table table-hover">
                 						<thead>
                 							<tr style="background-color:#FFFFFF;">
                 								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                                <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><input type="checkbox" id="subgroup_list_checkbox" onclick="subgroup_list_checkbox_click();"/></th>
                								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilspushnotification_subgroup;?></th>
                 							</tr>
                 						</thead>
                 						<tbody id="subgroup_list_content">

                 						</tbody>
                 					</table>
                                     </div>
                                 </div>
                                  <!-- /.tab-pane -->

                            <?php endif; ?>

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
                                    <?php echo $vilspushnotification_sendto ?>
                                  </h1>
                                    <table>
                                      <tbody>
                                        <tr>
                                          <td>
                                            <div id="send_node_list_ui">
                                            </div>
                                          </td>
                                        </tr>
                                      </tbody>
                                    </table>
                                </section>

                                <!-- Main content -->
                                <section class="content">

                                  <!-- row -->
                                  <div class="row">
                                    <div class="col-md-12">

                                        <!-- Horizontal Form -->
                                        <div class="box box-info">

                                          <!-- form start -->
                                          <form class="form-horizontal">
                                            <div class="box-body">

                                                <div class="form-group">
                                                    <label for="cannedmessage_type_title" class="col-sm-1 control-label">推播類型</label>
                                                  <div class="col-sm-10">
                                                      <!-- <div class="margin"> -->
                                                        <div class="btn-group">
                                                          <button type="button" class="btn btn-default btn-flat" id='cannedmessage_type_title'>不分類</button>
                                                          <button type="button" class="btn btn-default btn-flat dropdown-toggle" data-toggle="dropdown">
                                                            <span class="caret"></span>
                                                            <span class="sr-only">Toggle Dropdown</span>
                                                          </button>
                                                          <ul class="dropdown-menu" role="menu" id="cannedmessage_dropdownmenu">
                                                          </ul>
                                                        </div>
                                                      <!-- </div> -->
                                                  </div>
                                                </div>

                                              <div class="form-group">
                                                <label for="inputTitle" class="col-sm-1 control-label">標題</label>
                                                <div class="col-sm-10">
                                                  <input type="text" class="form-control" id="inputTitle" placeholder="推播標題">
                                                </div>
                                              </div>
                                              <div class="form-group">
                                                <label for="inputContext" class="col-sm-1 control-label">內容</label>
                                                 <div class="col-sm-10">
                                                  <textarea class="form-control" rows="10" id="inputContext" placeholder="推播內容"></textarea>
                                                </div>
                                              </div>
                                              <div class="form-group">
                                                <label for="inputImage" class="col-sm-1 control-label">照片</label>
                                                <div class="col-sm-10">
                                                     <div class="col-sm-3">
                                                      <form method="POST" enctype="multipart/form-data" action="php/uploadfile.php">
                                                         <div class="form-group">
                                                           <input type="text" class="form-control" id="imageDescription" placeholder="檔案說明">
                                                            <label for="imageUpload" class="control-sidebar-subheading">上傳檔案</label>
                                                            <input type="file" accept="image/*" id="imageUpload" name="imageUpload" onchange="imageSelected();">
                                                            <div id="imageFileSize"></div>
                                                            <div id="imageFileType"></div>
                                                            <div id="imageProgressNumber"></div>
                                                            <div id="removebutton" style="display:none;">
                                                                <button class="btn btn-danger" onclick="remove_uploadfile_click();return false;"><i class="fa fa-minus"></i></button>
                                                            </div>
                                                           </div>
                                                      </form>
                                                     </div>
                                                </div>

                                              </div>
                                              <div class="form-group">
                                                <label for="inputContext" class="col-sm-1 control-label">待辦事項</label>
                                                 <div class="col-sm-10">
                                                   <table class="table table-bordered" style="background-color:rgb(236, 240, 245);">
                                                     <thead style="display:none;">
                                                       <tr><th style="width: 50px"></th><th></th><th></th><th></th><th></th></tr>
                                                     </thead>
                                                     <tbody id="survey_table_body">
                                                     </tbody>
                                                   </table>
                                                   <div class="btn-group">
                                                     <button class="btn btn-block btn-success" onclick="create_survey_click();return false;"><i class="fa fa-plus"></i></button>
                                                   </div>
                                                </div>
                                              </div>
                                            </div>
                                            <!-- /.box-body -->
                                            <div class="box-footer">
                                                <div class="callout callout-warning" id="sendnotification_sending_callout" style="display:none">
                                                  <p>發出推播中...</p>
                                                </div>
                                                <div class="callout callout-danger" id="sendnotification_failed_callout" style="display:none">
                                                  <div id="sendnotification_failed_callout_msg"><p>發出推播 失敗!</p></div>
                                                </div>
                                                <div class="callout callout-success" id="sendnotification_finished_callout" style="display:none">
                                                  <p>發出推播 成功!</p>
                                                </div>

                                              <!-- <button type="submit" class="btn btn-default">Cancel</button> -->
                                              <button type="submit" class="btn btn-info pull-right" id="sendnotificationbtn">發出推播</button>
                                            </div>
                                            <!-- /.box-footer -->
                                          </form>
                                        </div>
                                        <!-- /.box -->
                                        <div class="col-sm-12">
                                          <div class="row">
                                            <div class="col-sm-8">
                                                <?php echo $vilspushnotification_android_sapp_dl;?>
                                                <a href='https://play.google.com/store/apps/details?id=com.smims.smims_vils_subscriber&pcampaignid=pcampaignidMKT-Other-global-all-co-prtnr-py-PartBadge-Mar2515-1' target='_blank'><img alt='Google Play立即下載' src='https://play.google.com/intl/en_us/badges/static/images/badges/zh-tw_badge_web_generic.png' width=300/></a>
                                            </div>
                                            <div class="col-sm-4">
                                                <img src='/images/VILS_Android_app.jpg?date=<?php echo $UPDATE_DATE;?>' alt='' class='margin' width='150'>
                                            </div>
                                          </div>
                                          <div class="row">
                                              <div class="col-sm-8">
                                                  <?php echo $vilspushnotification_ios_app_dl;?>
                                                  <a href='https://apps.apple.com/app/apple-store/id1507769058' target='_blank'><img alt='Apple App Store立即下載' src='/images/apple-store-icon-6.jpg' width=300/></a>
                                              </div>
                                              <div class="col-sm-4">
                                                  <img src='/images/VILS_iOS_app.jpg?date=<?php echo $UPDATE_DATE;?>' alt='' class='margin' width='150'>
                                              </div>
                                          </div>
                                        </div>

                                        <!-- <div class="col-sm-6">
                                            北瀚雲端通知 APP
                                            <a href='https://play.google.com/store/apps/details?id=com.smims.smims_vils_subscriber&pcampaignid=pcampaignidMKT-Other-global-all-co-prtnr-py-PartBadge-Mar2515-1' target='_blank'><img alt='Google Play立即下載' src='https://play.google.com/intl/en_us/badges/static/images/badges/zh-tw_badge_web_generic.png' width=300/></a>
                                            </p>
                                        </div>
                                        <div class="col-sm-2">
                                            <img src='/images/VILS_Android_app.jpg' alt='' class='margin' width='150'>
                                        </div> -->

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
        <!-- /.box box-success -->
        <!-- /.MAP & BOX PANE -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
      var gl_userid = "<?php echo $_SESSION["userid"] ?>";

      var send_nodeid = "<?php echo $nodeid ?>";
      var send_title = "<?php echo $title ?>";
      var send_message = "<?php echo $message ?>";

    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/pushnotification.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/pushnotification_src/00-datetimepicker.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/pushnotification_src/01-pushnotification.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage_src/02-uploadimage.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
