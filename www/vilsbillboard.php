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
    <title>SMIMS VILS IoT - Billboard</title>

    <style>
        .ui-datetimepicker { width: 10em; }
    </style>

  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsbillboard_title;?></h3>
            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_billboard_page();return false;"><i class="fa fa-external-link"></i>
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
                                <li class='active'><a href='#loc_tab_subgroup' data-toggle='tab'><span class='badge bg-green'> <?php echo $vilsbillboard_subgroup; ?></span></a></li>
                              </ul>

                              <div class="tab-content">

                               <div class="tab-pane active table-responsive" id="loc_tab_subgroup">
                                   <div style="height:650px;overflow:scroll">
                       					<table class="table table-hover">
                       						<thead>
                       							<tr style="background-color:#FFFFFF;">
                       								<th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;">#</th>
                                                      <th style="width:10px;position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><input type="checkbox" id="subgroup_list_checkbox" onclick="subgroup_list_checkbox_click();"/></th>
                      								<th style="position:sticky;position: -webkit-sticky;top: 0;background-color:#EEEEEE;"><?php echo $vilsbillboard_subgroup;?></th>
                       							</tr>
                       						</thead>
                       						<tbody id="subgroup_list_content">

                       						</tbody>
                       					</table>
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
                                    <?php echo $vilsbillboard_sendto ?>
                                  </h1>
                                    <table>
                                      <tbody>
                                        <tr>
                                          <td>
                                              <div id="send_node_list_ui"> </div>
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
                                                <label for="inputTitle" class="col-sm-1 control-label">標題</label>
                                                <div class="col-sm-10">
                                                  <input type="text" class="form-control" id="inputTitle" placeholder="公告標題">
                                                </div>
                                              </div>
                                              <div class="form-group">
                                                <label for="inputContext" class="col-sm-1 control-label">內容</label>
                                                 <div class="col-sm-10">
                                                  <textarea class="form-control" rows="10" id="inputContext" placeholder="公告內容"></textarea>
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
                                                <label for="sendNotify" class="col-sm-1 control-label">發送推播</label>
                                                <div class="col-sm-10">
                                                  <label>
                                                    <input type="checkbox" id="sendNotify">公告時推播</input>
                                                  </label>
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
                                              <button type="submit" class="btn btn-info pull-right" id="savebillboardbtn">發布公告</button>
                                            </div>
                                            <!-- /.box-footer -->
                                          </form>
                                        </div>
                                        <!-- /.box -->
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
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/billboard.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/billboard_src/00-billboard.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage_src/02-uploadimage.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
