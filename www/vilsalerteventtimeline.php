<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
  <link rel="shortcut icon" sizes="16x16 32x32 48x48 64x64" href="/favicon.ico">
  <link href="/css/vils.min.css" rel="stylesheet">
  <meta name="description" content="SMIMS VILS IoT">
  <meta name="author" content="SMIMS">

</head>

  <body class="hold-transition skin-blue sidebar-mini">
    <div class="wrapper">

    <!-- Content Wrapper. Contains page content -->
    <div class="content-wrapper">

    <!-- Main content -->
    <section class="content" style="margin:0; padding:0; border:0 none;">
        <div class="row">
          <!-- Left col -->
          <section class="col-lg-12 connectedSortable">

              <div id="alert-box-content">
             </div>

                <div class="box">
                  <div class="box-header with-border">
                    <h3 class="box-title"><?php echo $vilsalerteventtimeline_timeline;?></h3>
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
                              <!-- <li class="time-label">
                                <span class="bg-green">
                                  <button type="button" class="btn btn-default btn-lrg ajax" title="Ajax Request" id="AjaxLoadMode">
                                    <i class="fa fa-spin fa-refresh" id="AjaxLoadModeicon"></i>&nbsp; Load more...
                                  </button>
                                </span>

                              </li> -->
                              <!-- /.timeline-label -->

                              <li>
                                <i class="fa fa-clock-o bg-gray"></i>
                              </li>
                            </ul>

                    </div>
                    <!-- /.box-body -->
                  </div>
                  <!-- /.box -->


                  <!-- Users box -->
                  <div class="box box-solid bg-light-blue-gradient" style="display:none">
                      <div class="box-header">
                      <!-- tools box -->
                      <div class="pull-right box-tools">
                          <div class="btn-group">
                            <button type="button" class="btn btn-primary btn-sm dropdown-toggle" data-toggle="dropdown">
                              <i class="fa fa-bars"></i></button>
                              <ul class="dropdown-menu pull-right" role="menu" id="alerteventtimeline_deviceslist">
                              </ul>
                          </div>
                          <button type="button" class="btn btn-primary btn-sm" data-widget="collapse">
                          <i class="fa fa-minus"></i></button>
                      </div>
                      <!-- /. tools -->
                      <i class="fa fa-map-marker"></i>
                      <h3 class="box-title" id="alerteventtimeline_title"> <?php echo $vilsdevices_tag; ?> </h3>

                    </div>
                    <div class="box-body">

                        <div class="box box-widget widget-user">
                            <div class="widget-user-header bg-aqua-active">
                              <h3 class="widget-user-username" id="alerteventtimeline_username"><?php echo $vilsdevices_tag; ?></h3>
                              <h5 class="widget-user-desc" id="alerteventtimeline_desc">User</h5>
                            </div>
                            <div class="widget-user-image">
                              <img class="img-circle" src="/images/cat.png" alt="User Avatar">
                            </div>
                            <div class="box-footer bg-aqua">
                                      <div class="description-block">
                                        <h5 class="description-header" id="alerteventtimeline_areaname"></h5>
                                        <span class="description-text"></span>
                                      </div>
                                      <!-- /.description-block -->
                                      <div class="row">
                                        <div class="col-xs-4 text-center" style="border-right: 1px solid #f4f4f4">
                                          <div id="alerteventtimeline_alive">0</div>
                                          <div class="knob-label"><?php echo $vilsalerteventtimeline_alive;?></div>
                                        </div>
                                        <!-- ./col -->
                                        <div class="col-xs-4 text-center" style="border-right: 1px solid #f4f4f4">
                                          <div id="alerteventtimeline_floor">1</div>
                                          <div class="knob-label"><?php echo $vilsalerteventtimeline_floor;?></div>
                                        </div>
                                        <!-- ./col -->
                                        <div class="col-xs-4 text-center">
                                          <div id="alerteventtimeline_devinfo"></div>
                                        </div>
                                        <!-- ./col -->
                                      </div>
                                      <!-- /.row -->
                            </div>

                        </div>
                        <!-- /.widget-user -->

                    </div>
                    <!-- /.box-body-->
                  </div>
                  <!-- /.box -->





          </section>
          <!-- /.Left col -->
        </div>
        <!-- /.row -->
    </section>
    <!-- /.content -->
  </div>
  <!-- /.content-wrapper -->

  </div>
  <!-- ./wrapper -->

  <script>
    var project_id = <?php echo $project_id ?>;
    var accountid = "<?php echo $_SESSION["accountid"] ?>";
    var accountname = "<?php echo $_SESSION["accountname"] ?>";
    var groupid = "<?php echo $_SESSION["groupid"] ?>";
    var gl_maingroups = "<?php echo $_SESSION['maingroups'] ?>";
    var ALERT_BOX_FONT_SIZE = <?php echo $ALERT_BOX_FONT_SIZE ?>;
    console.log("vilsalerteventtimeline.php project_id="+project_id +", ALERT_BOX_FONT_SIZE:"+ALERT_BOX_FONT_SIZE);
  </script>

  <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/alerteventtimeline.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

  <!-- <script src="/js/alerteventtimeline_src/00-howler.core.js"></script>
  <script src="/js/alerteventtimeline_src/01-alerteventtimeline.js"></script>
  <script src="/js/alerteventtimeline_src/02-sosevent.js"></script>
  <script src="/js/alerteventtimeline_src/03-checkpointevent.js"></script>
  <script src="/js/alerteventtimeline_src/04-fencealertevent.js"></script>
  <script src="/js/alerteventtimeline_src/05-helpsound.js"></script>
  <script src="/js/alerteventtimeline_src/06-eventtimeline.js"></script> -->

  <script>
  //Make the alerteventtimeline widgets sortable Using jquery UI
  $(".connectedSortable").sortable({
    placeholder: "sort-highlight",
    connectWith: ".connectedSortable",
    handle: ".box-header, .nav-tabs",
    forcePlaceholderSize: true,
    zIndex: 999999
  });
  $(".connectedSortable .box-header, .connectedSortable .nav-tabs-custom").css("cursor", "move");

  //
  // auto click at first load page.
  //
  // document.getElementById("AjaxLoadMode").click();

  </script>


</body>

</html>
