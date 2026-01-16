`ifndef REPLACE_WITH_MODULE_AXILITE_TRACE_AGENT__SV
`define REPLACE_WITH_MODULE_AXILITE_TRACE_AGENT__SV

class REPLACE_WITH_MODULE_axilite_trace_monagent#(int ADDRW=64, int DATAW=64, int LENGTHW=8, int SIZEW=8, int IDW=1) extends REPLACE_WITH_MODULE_trace_base_component;

REPLACE_WITH_MODULE_axi_ctr_trace_phy_mondrv#(ADDRW, LENGTHW, SIZEW, IDW) AwPhy;
REPLACE_WITH_MODULE_axi_wdata_trace_phy_mondrv#(DATAW, IDW) WPhy;
REPLACE_WITH_MODULE_axi_brsp_trace_phy_mondrv#(IDW) BPhy;
REPLACE_WITH_MODULE_axi_ctr_trace_phy_mondrv#(ADDRW, LENGTHW, SIZEW, IDW) ArPhy;
REPLACE_WITH_MODULE_axi_rdata_trace_phy_mondrv#(DATAW, IDW) RPhy;

REPLACE_WITH_MODULE_axi_trace_collect Collect;

REPLACE_WITH_MODULE_cmd_trace_manager CmdMan; //only one memory manager; shared by all maxi interface

function new(string inst_name, string hier_name);
    super.new(inst_name, hier_name);
    this.AwPhy = new("AwPhy", this.full_name());
     this.WPhy = new("WPhy",  this.full_name());
     this.BPhy = new("BPhy",  this.full_name());
    this.ArPhy = new("ArPhy", this.full_name());
     this.RPhy = new("RPhy",  this.full_name());
    this.Collect=new("Collect",  this.full_name());
    this.CmdMan=new("CmdMan", this.full_name());
endfunction

virtual function void cfg(int cmd_manager_id, string mode, bit isaxiblkctrl, bit isblkctrlcmdm, bit ispipeline);
    string filename = this.full_name();
    for(int i=0; i<filename.len; i++) 
        if(filename[i]==".") filename[i] = "_";
    AwPhy  .cfg("aw_put", mode, "nodrv");
     WPhy  .cfg(mode, "nodrv");
     BPhy  .cfg(0, mode, "nodrv" );
    ArPhy  .cfg("ar_put", mode, "nodrv");
     RPhy  .cfg(0, mode, "nodrv");
     Collect.cfg(mode);
    CmdMan.cfg(cmd_manager_id, mode, filename, isaxiblkctrl, isblkctrlcmdm, ispipeline);
endfunction

virtual function void connect( virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) AwPhyIf,
                              virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(DATAW+DATAW/8+IDW+1)  WPhyIf,
                              virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(2+IDW)  BPhyIf,
                              virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2)  ArPhyIf,
                              virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(DATAW+IDW+3)  RPhyIf,
                              virtual REPLACE_WITH_MODULE_blk_ctrl_interface BlkCtrlIf
                             );
    AwPhy.add_puts(Collect);
     WPhy.add_puts(Collect);
     BPhy.add_puts(Collect);
    ArPhy.add_puts(Collect);
     RPhy.add_puts(Collect);
    Collect.add_puts(this.CmdMan);

    AwPhy.VldRdyDataIf = AwPhyIf;
     WPhy.VldRdyDataIf =  WPhyIf;
     BPhy.VldRdyDataIf =  BPhyIf;
    ArPhy.VldRdyDataIf = ArPhyIf;
     RPhy.VldRdyDataIf =  RPhyIf;
    CmdMan.BlkCtrlIf = BlkCtrlIf;
    if(CmdMan.mode=="tvdump") begin
        //BlkCtrlIf.file_fp.push_back(CmdMan.FpWrCmd);
        //BlkCtrlIf.file_fp.push_back(CmdMan.FpRdCmd);
        //BlkCtrlIf.file_fp.push_back(CmdMan.FpRdRes);
        BlkCtrlIf.cmd_man_q.push_back(CmdMan);
    end
endfunction

virtual task run();
    fork
        AwPhy.run();
         WPhy.run();
         BPhy.run();
        ArPhy.run();
         RPhy.run();
        Collect.run();
        CmdMan.run();
    join
endtask

endclass

class REPLACE_WITH_MODULE_axilite_trace_drvagent#(int ADDRW=64, int DATAW=64, int LENGTHW=8, int SIZEW=8, int IDW=1) extends REPLACE_WITH_MODULE_trace_base_component;

REPLACE_WITH_MODULE_axi_ctr_trace_phy_vlddrv#(ADDRW, LENGTHW, SIZEW, IDW) AwPhy;
REPLACE_WITH_MODULE_axi_wdata_trace_phy_vlddrv#(DATAW, IDW) WPhy;
REPLACE_WITH_MODULE_axi_brsp_trace_phy_rdydrv#(IDW) BPhy;
REPLACE_WITH_MODULE_axi_ctr_trace_phy_vlddrv#(ADDRW, LENGTHW, SIZEW, IDW) ArPhy;
REPLACE_WITH_MODULE_axi_rdata_trace_phy_rdydrv#(DATAW, IDW) RPhy;

REPLACE_WITH_MODULE_axi_trace_collect Collect;

REPLACE_WITH_MODULE_cmd_trace_manager CmdMan; //only one memory manager; shared by all maxi interface

function new(string inst_name, string hier_name);
    super.new(inst_name, hier_name);
    this.AwPhy = new("AwPhy", this.full_name());
     this.WPhy = new("WPhy",  this.full_name());
     this.BPhy = new("BPhy",  this.full_name());
    this.ArPhy = new("ArPhy", this.full_name());
     this.RPhy = new("RPhy",  this.full_name());
    this.Collect=new("Collect",  this.full_name());
    this.CmdMan=new("CmdMan", this.full_name());
endfunction

virtual function void cfg(int cmd_manager_id, string mode, bit isaxiblkctrl, bit isblkctrlcmdm, bit ispipeline);
    string filename = this.full_name();
    for(int i=0; i<filename.len; i++) 
        if(filename[i]==".") filename[i] = "_";
    AwPhy  .cfg("aw_put", mode, "vldrv");
     WPhy  .cfg(mode, "vlddrv");
     BPhy  .cfg(mode, "rdydrv" );
    ArPhy  .cfg("ar_put", mode, "vlddrv");
     RPhy  .cfg(mode, "rdydrv");
     Collect.cfg(mode);
    CmdMan.cfg(cmd_manager_id, mode, filename, isaxiblkctrl, isblkctrlcmdm, ispipeline);
endfunction

virtual function void connect( virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) AwPhyIf,
                              virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(DATAW+DATAW/8+IDW+1)  WPhyIf,
                              virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(2+IDW)  BPhyIf,
                              virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2)  ArPhyIf,
                              virtual REPLACE_WITH_MODULE_vld_rdy_data_trace_interface#(DATAW+IDW+3)  RPhyIf,
                              virtual REPLACE_WITH_MODULE_blk_ctrl_interface BlkCtrlIf
                             );
    AwPhy.add_puts(Collect);
     WPhy.add_puts(Collect);
     BPhy.add_puts(Collect);
    ArPhy.add_puts(Collect);
     RPhy.add_puts(Collect);
    Collect.add_puts(this.CmdMan);

    AwPhy.VldRdyDataIf = AwPhyIf;
     WPhy.VldRdyDataIf =  WPhyIf;
     BPhy.VldRdyDataIf =  BPhyIf;
    ArPhy.VldRdyDataIf = ArPhyIf;
     RPhy.VldRdyDataIf =  RPhyIf;
    CmdMan.BlkCtrlIf = BlkCtrlIf;
    
    CmdMan.add_puts(Collect);
    Collect.add_puts(AwPhy);
    Collect.add_puts( WPhy);
    Collect.add_puts( BPhy);
    Collect.add_puts(ArPhy);
    Collect.add_puts( RPhy);

endfunction

virtual task run();
    fork
        AwPhy.run();
         WPhy.run();
         BPhy.run();
        ArPhy.run();
         RPhy.run();
        Collect.run();
        CmdMan.run();
    join
endtask

endclass


`endif
