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
    <title>SMIMS VILS IoT - Health Examination</title>
  </head>

      <div class="modal fade" id="modal-savebodyinfo">
        <div class="modal-dialog">
          <div class="modal-content">
            <div class="modal-header">
              <button type="button" style="display:none" data-toggle="modal" data-target="#modal-savebodyinfo" id="opensavebodyinfomodqlbtn"></button>
              <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                <span aria-hidden="true">&times;</span></button>
              <h4 class="modal-title">儲存生理量測資訊</h4>
            </div>
            <div class="modal-body">
              <div>
                <div>
                  <label class="control-label">指定標籤:</label>
                  <div class="btn-group">
                    <button type="button" class="btn btn-default btn-flat" id="target_tag"><?php echo $vilsfootprint_select_tag;?></button>
                    <button type="button" class="btn btn-default btn-flat dropdown-toggle" data-toggle="dropdown">
                      <span class="caret"></span>
                      <span class="sr-only">Toggle Dropdown</span>
                    </button>
                    <ul class="dropdown-menu" role="menu" id="dropdown_tags">
                    </ul>
                  </div>
                </div>
                <div>
                  <input type="text" class="form-control" id="bodyinfocontent" readonly="true">
                </div>
               </div>
            </div>
            <div class="modal-footer">
                <div class="callout callout-warning" id="saveinfo_sending_callout" style="display:none">
                  <p>儲存資訊中...</p>
                </div>
                <div class="callout callout-danger" id="saveinfo_failed_callout" style="display:none">
                  <div id="sendAtcmd_failed_msg_callout"><p>儲存資訊 失敗!</p></div>
                </div>
                <div class="callout callout-success" id="saveinfo_finished_callout" style="display:none">
                  <p>儲存資訊 成功!</p>
                </div>

              <button type="button" class="btn btn-default" id="saveinfobtn">儲存資訊</button>
            </div>
          </div>
          <!-- /.modal-content -->
        </div>
        <!-- /.modal-dialog -->
      </div>
      <!-- /.modal -->


    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsbodyinfo_title;?></h3>
            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_bodyinfoforacare_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">

    		<div class="col-xs-12 col-sm-12 col-md-12 col-lg-12">
              <div class="row">
                <div class="box">
                    <div class="box-header with-border">
                    </div>
                    <div class="box-body no-padding">

                        <table class="table table-bordered">
                            <thead id="NODE_BODYINFO_TABLE_HEAD">
                            </thead>
                            <tbody id="NODE_BODYINFO_TABLE">
                            </tbody>
                        </table>
                    </div>
                   <!-- /.box-body -->
                </div>
                <!-- box  -->
             </div>
             <!-- row -->

           </div>

        </div>
        <!-- /.box-body -->
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
    <script src="/js/bodyinfoforacare.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/bodyinfoforacare_src/00-raphael.js"></script>
    <script src="/js/bodyinfoforacare_src/01-morris.js"></script>
    <script src="/js/bodyinfoforacare_src/02-jquery.knob.js"></script>
    <script src="/js/bodyinfoforacare_src/03-bodyinfoforacare.js"></script> -->

</html>
