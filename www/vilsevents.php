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
    <title>SMIMS VILS IoT - Events</title>
  </head>



        <div class="row" id="event_trig_edit" style="display:{{ event_editor_type }};">
          <div class="col-md-12">
            <!-- Event Settings -->
            <div class="box" id="event_trig_edit_box">
              <div class="box-header with-border">
                <h3 class="box-title"><?php echo $vilsevent_title;?></h3>
                <div class="box-tools pull-right">
                    <button type="button" class="btn btn-box-tool" onclick="open_events_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                    <!-- <a href="javascript:void(0)" onclick="open_events_page();return false;"><i class="fa fa-external-link"></i></a> -->
                </div>
              </div>
              <!-- /.box-header -->
              <div class="box-body" id="event_box_body">


              </div>
              <!-- /.box-body -->

              <div class="box-footer">

                  <!-- Split button -->
                  <div class="btn-group" id="event_tag_selection">
                  </div>
                  <div class="btn-group" id="event_action_selection">
                  </div>
                  <div class="btn-group" id="event_area_selection">
                  </div>
                  <div class="btn-group" id="event_time_selection">
                  </div>
                  <div class="btn-group pull-right">
                    <button type="button" onclick="event_trig_create();return false;" class="btn btn-default pull-right"><i class="fa fa-plus"></i> 新增事件</button>
                  </div>
              </div>
              <!-- /.box-footer-->
            </div>
            <!-- /.box-->
          </div>
          <!-- /.col -->
        </div>
        <!-- /.row -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/etrigger.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

</html>
