<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vils_html_head.php');

    if ($GL_CONFIG_ENABLE_MIN_SIDEBAR == 1)
    {
        echo '<body class="hold-transition skin-blue sidebar-collapse sidebar-mini">';
    }
    else
    {
        echo '<body class="hold-transition skin-blue                  sidebar-mini">';
    }
    ?>
  <!-- <body class="hold-transition skin-blue sidebar-collapse sidebar-mini"> -->


    <div class="wrapper">

    <?php
        $show_gear_button = false;
        include ('vils_header_gear.php');

        $dashboard_active = true;
        include ('left_side_bar.php');
    ?>

    <!-- Content Wrapper. Contains page content -->
    <div class="content-wrapper">

    <!-- Main content -->
    <section class="content" style="margin:0; padding:10; border:0 none;">
        <div class="row">
              <!-- Left col -->
              <div class="col-md-8 col-sm-8" id="main_left_frame">
                <iframe id="contentIframe" src="/index.php/vilsdashboard/<?php echo $project_id ?>" onLoad="" style="width:100%; height:100vh; margin:0; padding:0; border:0 none;" ></iframe>
              </div>
              <!-- /.Left col -->

              <!-- right column -->
              <div class="col-md-4 col-sm-4" id="main_right_frame">
                <iframe id="rightcontentIframe" src="/index.php/vilsalerteventtimeline/<?php echo $project_id ?>" onLoad="" style="width:100%; height:100vh; margin:0; padding:0; border:0 none;" ></iframe>
              </div>
              <!--/.col (right) -->
        </div>
        <!-- /.row -->


        <!-- <div class="row" style="display:none">
          <div class="col-xs-12">
            <div class="box box-default">
              <div class="box-header with-border">
                <h3 class="box-title">Tag Alert Modal</h3>
              </div>
              <div class="box-body">
                <button type="button" class="btn btn-danger" data-toggle="modal" data-target="#modal-danger" id="tagalertbtn">
                  Launch Alert Modal
                </button>
              </div>
            </div>
          </div>
        </div>

        <div class="modal modal-danger fade" id="modal-danger">
          <div class="modal-dialog">
            <div class="modal-content">
              <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-label="Close" onclick="tagalertclosebtn_onclick()" id="tagalertclosebtn">
                  <span aria-hidden="true">&times;</span></button>
                <h4 class="modal-title" id="tagalerttitle">Alert</h4>
              </div>
              <div class="modal-body">
                <p id="tagalertbody">Tag push alert button...</p>
              </div>
              <div class="modal-footer">
                <button type="button" class="btn btn-outline pull-left" data-dismiss="modal" onclick="tagalertclosebtn_onclick()">Close</button>
                <button type="button" class="btn btn-outline" style="display:none">Save changes</button>
              </div>
            </div>
          </div>
        </div> -->
        <!-- /.modal -->

    </section>
    <!-- /.content -->
  </div>
  <!-- /.content-wrapper -->


    <!-- Main Footer -->
    <footer class="main-footer">
      <!-- To the right -->
      <div class="pull-right hidden-xs">
      <strong> <?php echo $main_bottom_title ?></strong><b>&nbsp;Version</b> <?php echo $web_version ?>
      </div>

      <!-- Default to the left -->
      <div>
          <strong><?php echo $main_copyright_name ?></strong> <?php echo $main_copyright ?>
      </div>
    </footer>

  </div>
  <!-- ./wrapper -->

  <script>
    var project_id = <?php echo $project_id ?>;
    //console.log("vils.php project_id="+project_id);
    var accountid = "<?php echo $_SESSION["accountid"] ?>";
    var accountname = "<?php echo $_SESSION["accountname"] ?>";
    var groupid = "<?php echo $_SESSION["groupid"] ?>";
    console.log("vils.php accountid:"+accountid+", accountname:"+accountname+", groupid:"+groupid);
  </script>

  <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/vils.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

  <!-- <script src="/js/vils_src/00-vils.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/vils_src/01-checkeventfired.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</body>

</html>
