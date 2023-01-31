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
    <title>SMIMS VILS IoT - Events Timeline</title>
  </head>
      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilsalerteventtimeline_timeline ?></h3>
          <div class="box-tools pull-right">
            <!-- <a href="javascript:void(0)" onclick="open_eventtimeline_page();return false;"><i class="fa fa-external-link"></i></a> -->
            <button type="button" class="btn btn-box-tool" onclick="open_eventtimeline_page();return false;"><i class="fa fa-external-link"></i>
            </button>
            <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
            </button>
          </div>
        </div>
        <!-- /.box-header -->
        <div class="box-body">

                  <!-- The time line -->
                  <ul class="timeline">

                      <div class="ajax-content-0"></div>

                    <!-- timeline time label -->
                    <li class="time-label">
                      <span class="bg-green">
                        <button type="button" class="btn btn-default btn-lrg ajax" title="Ajax Request" id="AjaxLoadMode">
                          <i class="fa fa-spin fa-refresh" id="AjaxLoadModeicon"></i>&nbsp; <?php echo $vilsdevices_loading?>
                        </button>
                      </span>

                    </li>
                    <!-- /.timeline-label -->

                    <li>
                      <i class="fa fa-clock-o bg-gray"></i>
                    </li>
                  </ul>

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
    <script src="/js/etimeline.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/etimeline_src/00-eventtimeline.js"></script> -->

</body>

</html>
