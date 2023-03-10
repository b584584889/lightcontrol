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
    <style>
    #nodeinfoft {
        position: absolute;
        top: 2px;
        width: 50%;
        text-align: left;
        z-index: 100;
        display:block;
    }
    </style>

    <title>SMIMS VILS IoT - 3D Map</title>
  </head>

            <!-- MAP & BOX PANE -->
            <div class="box box-success">
              <div class="box-header with-border">
                <h3 class="box-title">WebGL 3D View</h3>
                <div class="box-tools pull-right">
                  <!-- <a href="javascript:void(0)" onclick="open_3dmap_page();return false;"><i class="fa fa-external-link"></i></a> -->
                  <button type="button" class="btn btn-box-tool" onclick="open_3dmap_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                  <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
                  </button>
                </div>
              </div>
              <!-- /.box-header -->
              <div class="box-body no-padding">
                <div class="row">
                  <div class="col-md-12 col-sm-12">
                    <div class="pad">
                        <!-- Map will be created here -->
                        <div id="nodeinfoft">
                            Node Info :  </br>
                            Name:<font color="red" id="nodename"></font></br>
                            Position:<font color="red" id="nodepos"></font></br>
                        </div>
                        <div id="content" style="height:87vh">
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="showtags" checked="true">?????? Tags
                          </label>
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="showmap" checked="true">????????????
                          </label>
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="showanchors">?????? Anchors
                          </label>
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="showcoordinators">?????? Coordinators
                          </label>
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="showlocators">?????? Locators
                          </label>
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="showaoas">?????? AoA
                          </label>
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="showAoARangingCloudPoints">??????AoA???????????????????????????
                          </label>
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="showAoAAngleVectorCloudPoints">??????AoA??????????????????
                          </label>
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="showAoAAngleCloudPoints">??????AoA??????????????????
                          </label>
                        </div>
                        <div class="checkbox">
                          <label>
                            <input type="checkbox" id="testAoAPowerTag">?????? AoA PowerTag????????????
                          </label>
                          <ul id="testAoAPowerTag_ul" style="display:none">
                              <li>
                                <label>?????????Tag??????(??????: Tagid:x,y,z)</label>
                                <input type="text" id="testAoAPowerTag_1_InputText">
                              </li>
                              <li>
                                <label>?????? ???Tag??????(??????: Tagid:x,y,z)</label>
                                <input type="text" id="testAoAPowerTag_2_InputText">
                              </li>
                              <li>
                                <label>?????????Tag??????(??????: Tagid:x,y,z)</label>
                                <input type="text" id="testAoAPowerTag_3_InputText">
                              </li>
                              <li>
                                <label>AoA PowerTag ID??????(??????: AoAid)</label>
                                <input type="text" id="testAoAPowerTag_aoaid_InputText">
                              </li>
                              <li>
                                <button class="btn btn-primary pull-left" id="updateAoAPowerTagInfobtn">??????</button>
                              </li>
                          </ul>
                        </div>
                    </div>
                  </div>
                  <!-- /.col -->
                </div>
                <!-- /.row -->
              </div>
              <!-- /.box-body -->
            </div>
            <!-- /.box box-success -->
            <!-- /.MAP & BOX PANE -->
          </div>
          <!-- /.col-md-12 -->
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
    <script src="/js/three2.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/threemap.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/three_src/03-OrbitControls.js"></script>
    <script src="/js/three_src/04-Projector.js"></script>
    <script src="/js/three_src/05-web3d.js"></script>
    <script src="/js/three_src/06-DragControls.js"></script>
    <script src="/js/three_src/07-TransformControls.js"></script> -->

</html>
