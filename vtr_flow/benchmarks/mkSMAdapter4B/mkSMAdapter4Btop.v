// Created by Verilog::EditFiles from mkSMAdapter4B.v
// Created by Verilog::EditFiles from mkSMAdapter4B.v



//

module mkSMAdapter4B(wciS0_Clk,
		     wciS0_MReset_n,

		     wciS0_MCmd,


		     wciS0_MAddrSpace,

		     wciS0_MByteEn,

		     wciS0_MAddr,

		     wciS0_MData,

		     wciS0_SResp,

		     wciS0_SData,

		     wciS0_SThreadBusy,

		     wciS0_SFlag,

		     wciS0_MFlag,

		     wmiM_MCmd,

		     wmiM_MReqLast,

		     wmiM_MReqInfo,

		     wmiM_MAddrSpace,

		     wmiM_MAddr,

		     wmiM_MBurstLength,

		     wmiM_MDataValid,

		     wmiM_MDataLast,

		     wmiM_MData,

		     wmiM_MDataByteEn,

		     wmiM_SResp,



		     wmiM_SThreadBusy,

		     wmiM_SDataThreadBusy,

		     wmiM_SRespLast,

		     wmiM_SFlag,

		     wmiM_MFlag,

		     wmiM_MReset_n,

		     wmiM_SReset_n,

		     wsiM1_MCmd,

		     wsiM1_MReqLast,

		     wsiM1_MBurstPrecise,

		     wsiM1_MBurstLength,

		     wsiM1_MData,

		     wsiM1_MByteEn,

		     wsiM1_MReqInfo,

		     wsiM1_SThreadBusy,

		     wsiM1_MReset_n,

		     wsiM1_SReset_n,

		     wsiS1_MCmd,

		     wsiS1_MReqLast,

		     wsiS1_MBurstPrecise,

		     wsiS1_MBurstLength,

		     wsiS1_MData,

		     wsiS1_MByteEn,

		     wsiS1_MReqInfo,

		     wsiS1_SThreadBusy,

		     wsiS1_SReset_n,

		     wsiS1_MReset_n,
			   
         prevent_sweep_node,
         
        /// wci_reqF_r_deq__whas,

         wmi_respF__ENQ,
         wmi_respF__DEQ,
         wmi_respF__D_OUT,
         wmi_respF__FULL_N,
         wmi_respF__EMPTY_N,
         wsiS_reqFifo__D_OUT,
         wsiS_reqFifo__ENQ,
         wsiS_reqFifo__DEQ,
         wsiS_reqFifo__FULL_N,
         wsiS_reqFifo__EMPTY_N
      //   wci_isReset__VAL,
      //   wmi_isReset__VAL,
       //  wsiM_isReset__VAL,
      //   wsiS_isReset__VAL
		
		      
		
       );
      ////
      // wmiM_SData
      ////
  parameter [31 : 0] smaCtrlInit = 32'b0;
  input  wciS0_Clk;
  input  wciS0_MReset_n;
  output prevent_sweep_node;
  output wmi_respF__ENQ;
  output wmi_respF__DEQ;
  input [31:0]  wmi_respF__D_OUT;
  input wmi_respF__FULL_N;
  input wmi_respF__EMPTY_N;
  input [60 : 0] wsiS_reqFifo__D_OUT;
  output wsiS_reqFifo__ENQ;
  output wsiS_reqFifo__DEQ;
  input wsiS_reqFifo__FULL_N;
  input wsiS_reqFifo__EMPTY_N;
  
  wire wci_isReset__VAL;
  wire wmi_isReset__VAL;
  wire wsiM_isReset__VAL;
  wire wsiS_isReset__VAL;

  
  // action method wciS0_mCmd
  input  [2 : 0] wciS0_MCmd;

  // action method wciS0_mAddrSpace
  input  wciS0_MAddrSpace;

  // action method wciS0_mByteEn
  input  [3 : 0] wciS0_MByteEn;

  // action method wciS0_mAddr
  input  [19 : 0] wciS0_MAddr;

  // action method wciS0_mData
  input  [31 : 0] wciS0_MData;

  // value method wciS0_sResp
  output [1 : 0] wciS0_SResp;

  // value method wciS0_sData
  output [31 : 0] wciS0_SData;

  // value method wciS0_sThreadBusy
  output wciS0_SThreadBusy;

  // value method wciS0_sFlag
  output [1 : 0] wciS0_SFlag;

  // action method wciS0_mFlag
  input  [1 : 0] wciS0_MFlag;

  // value method wmiM_mCmd
  output [2 : 0] wmiM_MCmd;

  // value method wmiM_mReqLast
  output wmiM_MReqLast;

  // value method wmiM_mReqInfo
  output wmiM_MReqInfo;

  // value method wmiM_mAddrSpace
  output wmiM_MAddrSpace;

  // value method wmiM_mAddr
  output [13 : 0] wmiM_MAddr;

  // value method wmiM_mBurstLength
  output [11 : 0] wmiM_MBurstLength;

  // value method wmiM_mDataValid
  output wmiM_MDataValid;

  // value method wmiM_mDataLast
  output wmiM_MDataLast;

  // value method wmiM_mData
  output [31 : 0] wmiM_MData;

  // value method wmiM_mDataInfo

  // value method wmiM_mDataByteEn
  output [3 : 0] wmiM_MDataByteEn;

  // action method wmiM_sResp
  input  [1 : 0] wmiM_SResp;

  // action method wmiM_sData
  ////
  //input  [31 : 0] wmiM_SData;
  ////
  // action method wmiM_sThreadBusy
  input  wmiM_SThreadBusy;

  // action method wmiM_sDataThreadBusy
  input  wmiM_SDataThreadBusy;

  // action method wmiM_sRespLast
  input  wmiM_SRespLast;

  // action method wmiM_sFlag
  input  [31 : 0] wmiM_SFlag;

  // value method wmiM_mFlag
  output [31 : 0] wmiM_MFlag;

  // value method wmiM_mReset_n
  output wmiM_MReset_n;

  // action method wmiM_sReset_n
  input  wmiM_SReset_n;

  // value method wsiM1_mCmd
  output [2 : 0] wsiM1_MCmd;

  // value method wsiM1_mReqLast
  output wsiM1_MReqLast;

  // value method wsiM1_mBurstPrecise
  output wsiM1_MBurstPrecise;

  // value method wsiM1_mBurstLength
  output [11 : 0] wsiM1_MBurstLength;

  // value method wsiM1_mData
  output [31 : 0] wsiM1_MData;

  // value method wsiM1_mByteEn
  output [3 : 0] wsiM1_MByteEn;

  // value method wsiM1_mReqInfo
  output [7 : 0] wsiM1_MReqInfo;

  // value method wsiM1_mDataInfo

  // action method wsiM1_sThreadBusy
  input  wsiM1_SThreadBusy;

  // value method wsiM1_mReset_n
  output wsiM1_MReset_n;

  // action method wsiM1_sReset_n
  input  wsiM1_SReset_n;

  // action method wsiS1_mCmd
  input  [2 : 0] wsiS1_MCmd;

  // action method wsiS1_mReqLast
  input  wsiS1_MReqLast;

  // action method wsiS1_mBurstPrecise
  input  wsiS1_MBurstPrecise;

  // action method wsiS1_mBurstLength
  input  [11 : 0] wsiS1_MBurstLength;

  // action method wsiS1_mData
  input  [31 : 0] wsiS1_MData;

  // action method wsiS1_mByteEn
  input  [3 : 0] wsiS1_MByteEn;

  // action method wsiS1_mReqInfo
  input  [7 : 0] wsiS1_MReqInfo;

  // action method wsiS1_mDataInfo

  // value method wsiS1_sThreadBusy
  output wsiS1_SThreadBusy;

  // value method wsiS1_sReset_n
  output wsiS1_SReset_n;

  // action method wsiS1_mReset_n
  input  wsiS1_MReset_n;

  // signals for module outputs
  wire [31 : 0] wciS0_SData, wmiM_MData, wmiM_MFlag, wsiM1_MData;
  wire [13 : 0] wmiM_MAddr;
  wire [11 : 0] wmiM_MBurstLength, wsiM1_MBurstLength;
  wire [7 : 0] wsiM1_MReqInfo;
  wire [3 : 0] wmiM_MDataByteEn, wsiM1_MByteEn;
  wire [2 : 0] wmiM_MCmd, wsiM1_MCmd;
  wire [1 : 0] wciS0_SFlag, wciS0_SResp;
  wire wciS0_SThreadBusy,
       wmiM_MAddrSpace,
       wmiM_MDataLast,
       wmiM_MDataValid,
       wmiM_MReqInfo,
       wmiM_MReqLast,
       wmiM_MReset_n,
       wsiM1_MBurstPrecise,
       wsiM1_MReqLast,
       wsiM1_MReset_n,
       wsiS1_SReset_n,
       wsiS1_SThreadBusy;

  // inlined wires
  wire [95 : 0] wsiM_extStatusW__wget, wsiS_extStatusW__wget;
  wire [60 : 0] wsiM_reqFifo_x_wire__wget; 
  
  ////
  //wire [59 : 0] wci_wciReq__wget;
  //wire [33 : 0] wmi_wmiResponse__wget;
  //wire [31 : 0] wmi_Em_sData_w__wget;
  //wire [60 : 0] wsiS_wsiReq__wget;
  ////
  wire [37 : 0] wmi_dhF_x_wire__wget;
  wire [33 : 0] wci_respF_x_wire__wget;
  wire [31 : 0] wci_Es_mData_w__wget,
		wmi_mFlagF_x_wire__wget,
		wmi_reqF_x_wire__wget,
		wsi_Es_mData_w__wget;
  wire [19 : 0] wci_Es_mAddr_w__wget;
  wire [11 : 0] wsi_Es_mBurstLength_w__wget;
  wire [7 : 0] wsi_Es_mReqInfo_w__wget;
  wire [3 : 0] fabRespCredit_acc_v1__wget,
	       fabRespCredit_acc_v2__wget,
	       wci_Es_mByteEn_w__wget,
	       wsi_Es_mByteEn_w__wget;
  wire [2 : 0] wci_Es_mCmd_w__wget, wci_wEdge__wget, wsi_Es_mCmd_w__wget;
  ////
  //wire [1 : 0] wmi_Em_sResp_w__wget;
  ////
  wire fabRespCredit_acc_v1__whas,
       fabRespCredit_acc_v2__whas,
       mesgPreRequest_1__wget,
       mesgPreRequest_1__whas,
       wci_Es_mAddrSpace_w__wget,
       wci_Es_mAddrSpace_w__whas,
       wci_Es_mAddr_w__whas,
       wci_Es_mByteEn_w__whas,
       wci_Es_mCmd_w__whas,
       wci_Es_mData_w__whas,
       wci_ctlAckReg_1__wget,
       wci_ctlAckReg_1__whas,
       wci_reqF_r_clr__whas,
       wci_respF_dequeueing__whas,
       wci_respF_enqueueing__whas,
       wci_respF_x_wire__whas,
       wci_sFlagReg_1__wget,
       wci_sFlagReg_1__whas,
       wci_sThreadBusy_pw__whas,
       wci_wEdge__whas,
       wci_wciReq__whas,
       wci_wci_cfrd_pw__whas,
       wci_wci_cfwr_pw__whas,
       wci_wci_ctrl_pw__whas,
       wmi_Em_sData_w__whas,
       wmi_Em_sResp_w__whas,
       wmi_dhF_dequeueing__whas,
       wmi_dhF_enqueueing__whas,
       wmi_dhF_x_wire__whas,
       wmi_mFlagF_dequeueing__whas,
       wmi_mFlagF_enqueueing__whas,
       wmi_mFlagF_x_wire__whas,
       wmi_operateD_1__wget,
       wmi_operateD_1__whas,
       wmi_peerIsReady_1__wget,
       wmi_peerIsReady_1__whas,
       wmi_reqF_dequeueing__whas,
       wmi_reqF_enqueueing__whas,
       wmi_reqF_x_wire__whas,
       wmi_sDataThreadBusy_d_1__wget,
       wmi_sDataThreadBusy_d_1__whas,
       wmi_sThreadBusy_d_1__wget,
       wmi_sThreadBusy_d_1__whas,
       wmi_wmiResponse__whas,
       wsiM_operateD_1__wget,
       wsiM_operateD_1__whas,
       wsiM_peerIsReady_1__wget,
       wsiM_peerIsReady_1__whas,
       wsiM_reqFifo_dequeueing__whas,
       wsiM_reqFifo_enqueueing__whas,
       wsiM_reqFifo_x_wire__whas,
       wsiM_sThreadBusy_pw__whas,
       wsiS_operateD_1__wget,
       wsiS_operateD_1__whas,
       wsiS_peerIsReady_1__wget,
       wsiS_peerIsReady_1__whas,
       wsiS_reqFifo_r_clr__whas,
       wsiS_reqFifo_r_deq__whas,
       wsiS_reqFifo_r_enq__whas,
       wsiS_wsiReq__whas,
       wsi_Es_mBurstLength_w__whas,
       wsi_Es_mBurstPrecise_w__whas,
       wsi_Es_mByteEn_w__whas,
       wsi_Es_mCmd_w__whas,
       wsi_Es_mDataInfo_w__whas,
       wsi_Es_mData_w__whas,
       wsi_Es_mReqInfo_w__whas,
       wsi_Es_mReqLast_w__whas;
////
//  wire wci_reqF_r_enq__whas;
////

////

//output wci_reqF_r_deq__whas;
wire wci_reqF_r_deq__whas;

input [59:0] wci_reqF__D_OUT;
input  wci_reqF__EMPTY_N;
////

  // register abortCount
  reg [31 : 0] abortCount;
  wire [31 : 0] abortCount__D_IN;
  wire abortCount__EN;

  // register doAbort
  reg doAbort;
  wire doAbort__D_IN, doAbort__EN;

  // register endOfMessage
  reg endOfMessage;
  wire endOfMessage__D_IN, endOfMessage__EN;

  // register errCount
  reg [31 : 0] errCount;
  wire [31 : 0] errCount__D_IN;
  wire errCount__EN;

  // register fabRespCredit_value
  reg [3 : 0] fabRespCredit_value;
  wire [3 : 0] fabRespCredit_value__D_IN;
  wire fabRespCredit_value__EN;

  // register fabWordsCurReq
  reg [13 : 0] fabWordsCurReq;
  wire [13 : 0] fabWordsCurReq__D_IN;
  wire fabWordsCurReq__EN;

  // register fabWordsRemain
  reg [13 : 0] fabWordsRemain;
  wire [13 : 0] fabWordsRemain__D_IN;
  wire fabWordsRemain__EN;

  // register firstMsgReq
  reg firstMsgReq;
  wire firstMsgReq__D_IN, firstMsgReq__EN;

  // register impreciseBurst
  reg impreciseBurst;
  reg impreciseBurst__D_IN;
  wire impreciseBurst__EN;

  // register lastMesg
  reg [31 : 0] lastMesg;
  wire [31 : 0] lastMesg__D_IN;
  wire lastMesg__EN;

  // register mesgCount
  reg [31 : 0] mesgCount;
  reg [31 : 0] mesgCount__D_IN;
  wire mesgCount__EN;

  // register mesgLength
  reg [14 : 0] mesgLength;
  reg [14 : 0] mesgLength__D_IN;
  wire mesgLength__EN;

  // register mesgLengthSoFar
  reg [13 : 0] mesgLengthSoFar;
  wire [13 : 0] mesgLengthSoFar__D_IN;
  wire mesgLengthSoFar__EN;

  // register mesgPreRequest
  reg mesgPreRequest;
  wire mesgPreRequest__D_IN, mesgPreRequest__EN;

  // register mesgReqAddr
  reg [13 : 0] mesgReqAddr;
  wire [13 : 0] mesgReqAddr__D_IN;
  wire mesgReqAddr__EN;

  // register mesgReqOK
  reg mesgReqOK;
  wire mesgReqOK__D_IN, mesgReqOK__EN;

  // register mesgReqValid
  reg mesgReqValid;
  wire mesgReqValid__D_IN, mesgReqValid__EN;

  // register opcode
  reg [8 : 0] opcode;
  reg [8 : 0] opcode__D_IN;
  wire opcode__EN;

  // register preciseBurst
  reg preciseBurst;
  reg preciseBurst__D_IN;
  wire preciseBurst__EN;

  // register readyToPush
  reg readyToPush;
  reg readyToPush__D_IN;
  wire readyToPush__EN;

  // register readyToRequest
  reg readyToRequest;
  wire readyToRequest__D_IN, readyToRequest__EN;

  // register smaCtrl
  reg [31 : 0] smaCtrl;
  wire [31 : 0] smaCtrl__D_IN;
  wire smaCtrl__EN;

  // register thisMesg
  reg [31 : 0] thisMesg;
  reg [31 : 0] thisMesg__D_IN;
  wire thisMesg__EN;

  // register unrollCnt
  reg [15 : 0] unrollCnt;
  wire [15 : 0] unrollCnt__D_IN;
  wire unrollCnt__EN;

  // register valExpect
  reg [31 : 0] valExpect;
  wire [31 : 0] valExpect__D_IN;
  wire valExpect__EN;

  // register wci_cEdge
  reg [2 : 0] wci_cEdge;
  wire [2 : 0] wci_cEdge__D_IN;
  wire wci_cEdge__EN;

  // register wci_cState
  reg [2 : 0] wci_cState;
  wire [2 : 0] wci_cState__D_IN;
  wire wci_cState__EN;

  // register wci_ctlAckReg
  reg wci_ctlAckReg;
  wire wci_ctlAckReg__D_IN, wci_ctlAckReg__EN;

  // register wci_ctlOpActive
  reg wci_ctlOpActive;
  wire wci_ctlOpActive__D_IN, wci_ctlOpActive__EN;

  // register wci_illegalEdge
  reg wci_illegalEdge;
  wire wci_illegalEdge__D_IN, wci_illegalEdge__EN;

  // register wci_nState
  reg [2 : 0] wci_nState;
  reg [2 : 0] wci_nState__D_IN;
  wire wci_nState__EN;

  // register wci_reqF_countReg
  reg [1 : 0] wci_reqF_countReg;
  wire [1 : 0] wci_reqF_countReg__D_IN;
  wire wci_reqF_countReg__EN;

  // register wci_respF_c_r
  reg [1 : 0] wci_respF_c_r;
  wire [1 : 0] wci_respF_c_r__D_IN;
  wire wci_respF_c_r__EN;

  // register wci_respF_q_0
  reg [33 : 0] wci_respF_q_0;
  reg [33 : 0] wci_respF_q_0__D_IN;
  wire wci_respF_q_0__EN;

  // register wci_respF_q_1
  reg [33 : 0] wci_respF_q_1;
  reg [33 : 0] wci_respF_q_1__D_IN;
  wire wci_respF_q_1__EN;

  // register wci_sFlagReg
  reg wci_sFlagReg;
  wire wci_sFlagReg__D_IN, wci_sFlagReg__EN;

  // register wci_sThreadBusy_d
  reg wci_sThreadBusy_d;
  wire wci_sThreadBusy_d__D_IN, wci_sThreadBusy_d__EN;

  // register wmi_busyWithMessage
  reg wmi_busyWithMessage;
  wire wmi_busyWithMessage__D_IN, wmi_busyWithMessage__EN;

  // register wmi_dhF_c_r
  reg [1 : 0] wmi_dhF_c_r;
  wire [1 : 0] wmi_dhF_c_r__D_IN;
  wire wmi_dhF_c_r__EN;

  // register wmi_dhF_q_0
  reg [37 : 0] wmi_dhF_q_0;
  reg [37 : 0] wmi_dhF_q_0__D_IN;
  wire wmi_dhF_q_0__EN;

  // register wmi_dhF_q_1
  reg [37 : 0] wmi_dhF_q_1;
  reg [37 : 0] wmi_dhF_q_1__D_IN;
  wire wmi_dhF_q_1__EN;

  // register wmi_mFlagF_c_r
  reg [1 : 0] wmi_mFlagF_c_r;
  wire [1 : 0] wmi_mFlagF_c_r__D_IN;
  wire wmi_mFlagF_c_r__EN;

  // register wmi_mFlagF_q_0
  reg [31 : 0] wmi_mFlagF_q_0;
  reg [31 : 0] wmi_mFlagF_q_0__D_IN;
  wire wmi_mFlagF_q_0__EN;

  // register wmi_mFlagF_q_1
  reg [31 : 0] wmi_mFlagF_q_1;
  reg [31 : 0] wmi_mFlagF_q_1__D_IN;
  wire wmi_mFlagF_q_1__EN;

  // register wmi_operateD
  reg wmi_operateD;
  wire wmi_operateD__D_IN, wmi_operateD__EN;

  // register wmi_peerIsReady
  reg wmi_peerIsReady;
  wire wmi_peerIsReady__D_IN, wmi_peerIsReady__EN;

  // register wmi_reqF_c_r
  reg [1 : 0] wmi_reqF_c_r;
  wire [1 : 0] wmi_reqF_c_r__D_IN;
  wire wmi_reqF_c_r__EN;

  // register wmi_reqF_q_0
  reg [31 : 0] wmi_reqF_q_0;
  reg [31 : 0] wmi_reqF_q_0__D_IN;
  wire wmi_reqF_q_0__EN;

  // register wmi_reqF_q_1
  reg [31 : 0] wmi_reqF_q_1;
  reg [31 : 0] wmi_reqF_q_1__D_IN;
  wire wmi_reqF_q_1__EN;

  // register wmi_sDataThreadBusy_d
  reg wmi_sDataThreadBusy_d;
  wire wmi_sDataThreadBusy_d__D_IN, wmi_sDataThreadBusy_d__EN;

  // register wmi_sFlagReg
  reg [31 : 0] wmi_sFlagReg;
  wire [31 : 0] wmi_sFlagReg__D_IN;
  wire wmi_sFlagReg__EN;

  // register wmi_sThreadBusy_d
  reg wmi_sThreadBusy_d;
  wire wmi_sThreadBusy_d__D_IN, wmi_sThreadBusy_d__EN;

  // register wsiM_burstKind
  reg [1 : 0] wsiM_burstKind;
  wire [1 : 0] wsiM_burstKind__D_IN;
  wire wsiM_burstKind__EN;

  // register wsiM_errorSticky
  reg wsiM_errorSticky;
  wire wsiM_errorSticky__D_IN, wsiM_errorSticky__EN;

  // register wsiM_iMesgCount
  reg [31 : 0] wsiM_iMesgCount;
  wire [31 : 0] wsiM_iMesgCount__D_IN;
  wire wsiM_iMesgCount__EN;

  // register wsiM_operateD
  reg wsiM_operateD;
  wire wsiM_operateD__D_IN, wsiM_operateD__EN;

  // register wsiM_pMesgCount
  reg [31 : 0] wsiM_pMesgCount;
  wire [31 : 0] wsiM_pMesgCount__D_IN;
  wire wsiM_pMesgCount__EN;

  // register wsiM_peerIsReady
  reg wsiM_peerIsReady;
  wire wsiM_peerIsReady__D_IN, wsiM_peerIsReady__EN;

  // register wsiM_reqFifo_c_r
  reg [1 : 0] wsiM_reqFifo_c_r;
  wire [1 : 0] wsiM_reqFifo_c_r__D_IN;
  wire wsiM_reqFifo_c_r__EN;

  // register wsiM_reqFifo_q_0
  reg [60 : 0] wsiM_reqFifo_q_0;
  reg [60 : 0] wsiM_reqFifo_q_0__D_IN;
  wire wsiM_reqFifo_q_0__EN;

  // register wsiM_reqFifo_q_1
  reg [60 : 0] wsiM_reqFifo_q_1;
  reg [60 : 0] wsiM_reqFifo_q_1__D_IN;
  wire wsiM_reqFifo_q_1__EN;

  // register wsiM_sThreadBusy_d
  reg wsiM_sThreadBusy_d;
  wire wsiM_sThreadBusy_d__D_IN, wsiM_sThreadBusy_d__EN;

  // register wsiM_statusR
  reg [7 : 0] wsiM_statusR;
  wire [7 : 0] wsiM_statusR__D_IN;
  wire wsiM_statusR__EN;

  // register wsiM_tBusyCount
  reg [31 : 0] wsiM_tBusyCount;
  wire [31 : 0] wsiM_tBusyCount__D_IN;
  wire wsiM_tBusyCount__EN;

  // register wsiM_trafficSticky
  reg wsiM_trafficSticky;
  wire wsiM_trafficSticky__D_IN, wsiM_trafficSticky__EN;

  // register wsiS_burstKind
  reg [1 : 0] wsiS_burstKind;
  wire [1 : 0] wsiS_burstKind__D_IN;
  wire wsiS_burstKind__EN;

  // register wsiS_errorSticky
  reg wsiS_errorSticky;
  wire wsiS_errorSticky__D_IN, wsiS_errorSticky__EN;

  // register wsiS_iMesgCount
  reg [31 : 0] wsiS_iMesgCount;
  wire [31 : 0] wsiS_iMesgCount__D_IN;
  wire wsiS_iMesgCount__EN;

  // register wsiS_operateD
  reg wsiS_operateD;
  wire wsiS_operateD__D_IN, wsiS_operateD__EN;

  // register wsiS_pMesgCount
  reg [31 : 0] wsiS_pMesgCount;
  wire [31 : 0] wsiS_pMesgCount__D_IN;
  wire wsiS_pMesgCount__EN;

  // register wsiS_peerIsReady
  reg wsiS_peerIsReady;
  wire wsiS_peerIsReady__D_IN, wsiS_peerIsReady__EN;

  // register wsiS_reqFifo_countReg
  reg [1 : 0] wsiS_reqFifo_countReg;
  wire [1 : 0] wsiS_reqFifo_countReg__D_IN;
  wire wsiS_reqFifo_countReg__EN;

  // register wsiS_statusR
  reg [7 : 0] wsiS_statusR;
  wire [7 : 0] wsiS_statusR__D_IN;
  wire wsiS_statusR__EN;

  // register wsiS_tBusyCount
  reg [31 : 0] wsiS_tBusyCount;
  wire [31 : 0] wsiS_tBusyCount__D_IN;
  wire wsiS_tBusyCount__EN;

  // register wsiS_trafficSticky
  reg wsiS_trafficSticky;
  wire wsiS_trafficSticky__D_IN, wsiS_trafficSticky__EN;

  // register wsiWordsRemain
  reg [11 : 0] wsiWordsRemain;
  wire [11 : 0] wsiWordsRemain__D_IN;
  wire wsiWordsRemain__EN;

  // register zeroLengthMesg
  reg zeroLengthMesg;
  wire zeroLengthMesg__D_IN, zeroLengthMesg__EN;

  // ports of submodule wci_isReset


  // ports of submodule wci_reqF
  wire [59 : 0] wci_reqF__D_IN;
  wire [59 : 0] wci_reqF__D_OUT;
  
////
//wire wci_reqF__CLR, wci_reqF__DEQ, wci_reqF__ENQ;
//wire [31 : 0] wmi_respF__D_IN;
//wire wmi_respF__CLR;
//wire wmi_respF__DEQ;
//wire wmi_respF__ENQ;
////
  // ports of submodule wmi_isReset


  // ports of submodule wmi_respF
  
  
  


  // ports of submodule wsiM_isReset


  // ports of submodule wsiS_isReset
  

  // ports of submodule wsiS_reqFifo
  ////
  //wire [60 : 0] wsiS_reqFifo__D_IN;
  
  // wire wsiS_reqFifo__CLR;
  ////
  



  // rule scheduling signals
  wire CAN_FIRE_RL_fabRespCredit_accumulate,
       CAN_FIRE_RL_mesgPreRequest__dreg_update,
       CAN_FIRE_RL_operating_actions,
       CAN_FIRE_RL_wci_Es_doAlways_Req,
       CAN_FIRE_RL_wci_cfrd,
       CAN_FIRE_RL_wci_cfwr,
       CAN_FIRE_RL_wci_ctlAckReg__dreg_update,
       CAN_FIRE_RL_wci_ctl_op_complete,
       CAN_FIRE_RL_wci_ctl_op_start,
       CAN_FIRE_RL_wci_ctrl_EiI,
       CAN_FIRE_RL_wci_ctrl_IsO,
       CAN_FIRE_RL_wci_ctrl_OrE,
       CAN_FIRE_RL_wci_reqF__updateLevelCounter,
       CAN_FIRE_RL_wci_request_decode,
       CAN_FIRE_RL_wci_respF_both,
       CAN_FIRE_RL_wci_respF_decCtr,
       CAN_FIRE_RL_wci_respF_deq,
       CAN_FIRE_RL_wci_respF_incCtr,
       CAN_FIRE_RL_wci_sFlagReg__dreg_update,
       CAN_FIRE_RL_wci_sThreadBusy_reg,
       CAN_FIRE_RL_wmi_Em_doAlways,
       CAN_FIRE_RL_wmi_dhF_both,
       CAN_FIRE_RL_wmi_dhF_decCtr,
       CAN_FIRE_RL_wmi_dhF_deq,
       CAN_FIRE_RL_wmi_dhF_incCtr,
       CAN_FIRE_RL_wmi_mFlagF_both,
       CAN_FIRE_RL_wmi_mFlagF_decCtr,
       CAN_FIRE_RL_wmi_mFlagF_incCtr,
       CAN_FIRE_RL_wmi_operateD__dreg_update,
       CAN_FIRE_RL_wmi_peerIsReady__dreg_update,
       CAN_FIRE_RL_wmi_reqF_both,
       CAN_FIRE_RL_wmi_reqF_decCtr,
       CAN_FIRE_RL_wmi_reqF_deq,
       CAN_FIRE_RL_wmi_reqF_incCtr,
       CAN_FIRE_RL_wmi_respAdvance,
       CAN_FIRE_RL_wmi_sDataThreadBusy_d__dreg_update,
       CAN_FIRE_RL_wmi_sThreadBusy_d__dreg_update,
       CAN_FIRE_RL_wmrd_mesgBegin,
       CAN_FIRE_RL_wmrd_mesgBodyPreRequest,
       CAN_FIRE_RL_wmrd_mesgBodyRequest,
       CAN_FIRE_RL_wmrd_mesgBodyResponse,
       CAN_FIRE_RL_wmwt_doAbort,
       CAN_FIRE_RL_wmwt_mesgBegin,
       CAN_FIRE_RL_wmwt_messageFinalize,
       CAN_FIRE_RL_wmwt_messagePushImprecise,
       CAN_FIRE_RL_wmwt_messagePushPrecise,
       CAN_FIRE_RL_wmwt_requestPrecise,
       CAN_FIRE_RL_wsiM_ext_status_assign,
       CAN_FIRE_RL_wsiM_inc_tBusyCount,
       CAN_FIRE_RL_wsiM_operateD__dreg_update,
       CAN_FIRE_RL_wsiM_peerIsReady__dreg_update,
       CAN_FIRE_RL_wsiM_reqFifo_both,
       CAN_FIRE_RL_wsiM_reqFifo_decCtr,
       CAN_FIRE_RL_wsiM_reqFifo_deq,
       CAN_FIRE_RL_wsiM_reqFifo_incCtr,
       CAN_FIRE_RL_wsiM_sThreadBusy_reg,
       CAN_FIRE_RL_wsiM_update_statusR,
       CAN_FIRE_RL_wsiS_ext_status_assign,
       CAN_FIRE_RL_wsiS_inc_tBusyCount,
       CAN_FIRE_RL_wsiS_operateD__dreg_update,
       CAN_FIRE_RL_wsiS_peerIsReady__dreg_update,
       CAN_FIRE_RL_wsiS_reqFifo__updateLevelCounter,
       CAN_FIRE_RL_wsiS_reqFifo_enq,
       CAN_FIRE_RL_wsiS_update_statusR,
       CAN_FIRE_RL_wsi_Es_doAlways,
       CAN_FIRE_RL_wsipass_doMessagePush,
       CAN_FIRE_wciS0_mAddr,
       CAN_FIRE_wciS0_mAddrSpace,
       CAN_FIRE_wciS0_mByteEn,
       CAN_FIRE_wciS0_mCmd,
       CAN_FIRE_wciS0_mData,
       CAN_FIRE_wciS0_mFlag,
       CAN_FIRE_wmiM_sData,
       CAN_FIRE_wmiM_sDataThreadBusy,
       CAN_FIRE_wmiM_sFlag,
       CAN_FIRE_wmiM_sReset_n,
       CAN_FIRE_wmiM_sResp,
       CAN_FIRE_wmiM_sRespLast,
       CAN_FIRE_wmiM_sThreadBusy,
       CAN_FIRE_wsiM1_sReset_n,
       CAN_FIRE_wsiM1_sThreadBusy,
       CAN_FIRE_wsiS1_mBurstLength,
       CAN_FIRE_wsiS1_mBurstPrecise,
       CAN_FIRE_wsiS1_mByteEn,
       CAN_FIRE_wsiS1_mCmd,
       CAN_FIRE_wsiS1_mData,
       CAN_FIRE_wsiS1_mDataInfo,
       CAN_FIRE_wsiS1_mReqInfo,
       CAN_FIRE_wsiS1_mReqLast,
       CAN_FIRE_wsiS1_mReset_n,
       WILL_FIRE_RL_fabRespCredit_accumulate,
       WILL_FIRE_RL_mesgPreRequest__dreg_update,
       WILL_FIRE_RL_operating_actions,
       WILL_FIRE_RL_wci_Es_doAlways_Req,
       WILL_FIRE_RL_wci_cfrd,
       WILL_FIRE_RL_wci_cfwr,
       WILL_FIRE_RL_wci_ctlAckReg__dreg_update,
       WILL_FIRE_RL_wci_ctl_op_complete,
       WILL_FIRE_RL_wci_ctl_op_start,
       WILL_FIRE_RL_wci_ctrl_EiI,
       WILL_FIRE_RL_wci_ctrl_IsO,
       WILL_FIRE_RL_wci_ctrl_OrE,
       WILL_FIRE_RL_wci_reqF__updateLevelCounter,
       WILL_FIRE_RL_wci_request_decode,
       WILL_FIRE_RL_wci_respF_both,
       WILL_FIRE_RL_wci_respF_decCtr,
       WILL_FIRE_RL_wci_respF_deq,
       WILL_FIRE_RL_wci_respF_incCtr,
       WILL_FIRE_RL_wci_sFlagReg__dreg_update,
       WILL_FIRE_RL_wci_sThreadBusy_reg,
       WILL_FIRE_RL_wmi_Em_doAlways,
       WILL_FIRE_RL_wmi_dhF_both,
       WILL_FIRE_RL_wmi_dhF_decCtr,
       WILL_FIRE_RL_wmi_dhF_deq,
       WILL_FIRE_RL_wmi_dhF_incCtr,
       WILL_FIRE_RL_wmi_mFlagF_both,
       WILL_FIRE_RL_wmi_mFlagF_decCtr,
       WILL_FIRE_RL_wmi_mFlagF_incCtr,
       WILL_FIRE_RL_wmi_operateD__dreg_update,
       WILL_FIRE_RL_wmi_peerIsReady__dreg_update,
       WILL_FIRE_RL_wmi_reqF_both,
       WILL_FIRE_RL_wmi_reqF_decCtr,
       WILL_FIRE_RL_wmi_reqF_deq,
       WILL_FIRE_RL_wmi_reqF_incCtr,
       WILL_FIRE_RL_wmi_respAdvance,
       WILL_FIRE_RL_wmi_sDataThreadBusy_d__dreg_update,
       WILL_FIRE_RL_wmi_sThreadBusy_d__dreg_update,
       WILL_FIRE_RL_wmrd_mesgBegin,
       WILL_FIRE_RL_wmrd_mesgBodyPreRequest,
       WILL_FIRE_RL_wmrd_mesgBodyRequest,
       WILL_FIRE_RL_wmrd_mesgBodyResponse,
       WILL_FIRE_RL_wmwt_doAbort,
       WILL_FIRE_RL_wmwt_mesgBegin,
       WILL_FIRE_RL_wmwt_messageFinalize,
       WILL_FIRE_RL_wmwt_messagePushImprecise,
       WILL_FIRE_RL_wmwt_messagePushPrecise,
       WILL_FIRE_RL_wmwt_requestPrecise,
       WILL_FIRE_RL_wsiM_ext_status_assign,
       WILL_FIRE_RL_wsiM_inc_tBusyCount,
       WILL_FIRE_RL_wsiM_operateD__dreg_update,
       WILL_FIRE_RL_wsiM_peerIsReady__dreg_update,
       WILL_FIRE_RL_wsiM_reqFifo_both,
       WILL_FIRE_RL_wsiM_reqFifo_decCtr,
       WILL_FIRE_RL_wsiM_reqFifo_deq,
       WILL_FIRE_RL_wsiM_reqFifo_incCtr,
       WILL_FIRE_RL_wsiM_sThreadBusy_reg,
       WILL_FIRE_RL_wsiM_update_statusR,
       WILL_FIRE_RL_wsiS_ext_status_assign,
       WILL_FIRE_RL_wsiS_inc_tBusyCount,
       WILL_FIRE_RL_wsiS_operateD__dreg_update,
       WILL_FIRE_RL_wsiS_peerIsReady__dreg_update,
       WILL_FIRE_RL_wsiS_reqFifo__updateLevelCounter,
       WILL_FIRE_RL_wsiS_reqFifo_enq,
       WILL_FIRE_RL_wsiS_update_statusR,
       WILL_FIRE_RL_wsi_Es_doAlways,
       WILL_FIRE_RL_wsipass_doMessagePush,
       WILL_FIRE_wciS0_mAddr,
       WILL_FIRE_wciS0_mAddrSpace,
       WILL_FIRE_wciS0_mByteEn,
       WILL_FIRE_wciS0_mCmd,
       WILL_FIRE_wciS0_mData,
       WILL_FIRE_wciS0_mFlag,
       WILL_FIRE_wmiM_sData,
       WILL_FIRE_wmiM_sDataThreadBusy,
       WILL_FIRE_wmiM_sFlag,
       WILL_FIRE_wmiM_sReset_n,
       WILL_FIRE_wmiM_sResp,
       WILL_FIRE_wmiM_sRespLast,
       WILL_FIRE_wmiM_sThreadBusy,
       WILL_FIRE_wsiM1_sReset_n,
       WILL_FIRE_wsiM1_sThreadBusy,
       WILL_FIRE_wsiS1_mBurstLength,
       WILL_FIRE_wsiS1_mBurstPrecise,
       WILL_FIRE_wsiS1_mByteEn,
       WILL_FIRE_wsiS1_mCmd,
       WILL_FIRE_wsiS1_mData,
       WILL_FIRE_wsiS1_mDataInfo,
       WILL_FIRE_wsiS1_mReqInfo,
       WILL_FIRE_wsiS1_mReqLast,
       WILL_FIRE_wsiS1_mReset_n;
////  
//  wire WILL_FIRE_RL_wci_reqF_enq, CAN_FIRE_RL_wci_reqF_enq;
////

  // inputs to muxes for submodule ports
  reg [33 : 0] MUX_wci_respF_q_0__write_1__VAL_1;
  reg [31 : 0] MUX_wmi_reqF_q_0__write_1__VAL_1;
  wire [60 : 0] MUX_wsiM_reqFifo_q_0__write_1__VAL_1,
		MUX_wsiM_reqFifo_q_0__write_1__VAL_2,
		MUX_wsiM_reqFifo_q_1__write_1__VAL_1,
		MUX_wsiM_reqFifo_x_wire__wset_1__VAL_1;
  wire [37 : 0] MUX_wmi_dhF_q_0__write_1__VAL_1,
		MUX_wmi_dhF_q_0__write_1__VAL_2,
		MUX_wmi_dhF_q_1__write_1__VAL_1,
		MUX_wmi_dhF_x_wire__wset_1__VAL_1,
		MUX_wmi_dhF_x_wire__wset_1__VAL_2;
  wire [33 : 0] MUX_wci_respF_q_0__write_1__VAL_2,
		MUX_wci_respF_q_1__write_1__VAL_2,
		MUX_wci_respF_x_wire__wset_1__VAL_1,
		MUX_wci_respF_x_wire__wset_1__VAL_2;
  wire [31 : 0] MUX_mesgCount__write_1__VAL_1,
		MUX_thisMesg__write_1__VAL_1,
		MUX_thisMesg__write_1__VAL_2,
		MUX_wmi_mFlagF_q_0__write_1__VAL_1,
		MUX_wmi_mFlagF_q_1__write_1__VAL_1,
		MUX_wmi_mFlagF_x_wire__wset_1__VAL_1,
		MUX_wmi_mFlagF_x_wire__wset_1__VAL_3,
		MUX_wmi_reqF_q_0__write_1__VAL_2,
		MUX_wmi_reqF_q_1__write_1__VAL_2,
		MUX_wmi_reqF_x_wire__wset_1__VAL_1,
		MUX_wmi_reqF_x_wire__wset_1__VAL_2,
		MUX_wmi_reqF_x_wire__wset_1__VAL_3;
  wire [15 : 0] MUX_unrollCnt__write_1__VAL_1, MUX_unrollCnt__write_1__VAL_2;
  wire [14 : 0] MUX_mesgLength__write_1__VAL_2, MUX_mesgLength__write_1__VAL_4;
  wire [13 : 0] MUX_fabWordsRemain__write_1__VAL_1,
		MUX_fabWordsRemain__write_1__VAL_2,
		MUX_mesgReqAddr__write_1__VAL_2;
  wire [11 : 0] MUX_wsiWordsRemain__write_1__VAL_2;
  wire [8 : 0] MUX_opcode__write_1__VAL_2;
  wire [3 : 0] MUX_fabRespCredit_value__write_1__VAL_2;
  wire [1 : 0] MUX_wci_respF_c_r__write_1__VAL_1,
	       MUX_wci_respF_c_r__write_1__VAL_2,
	       MUX_wmi_dhF_c_r__write_1__VAL_1,
	       MUX_wmi_dhF_c_r__write_1__VAL_2,
	       MUX_wmi_mFlagF_c_r__write_1__VAL_1,
	       MUX_wmi_mFlagF_c_r__write_1__VAL_2,
	       MUX_wmi_reqF_c_r__write_1__VAL_1,
	       MUX_wmi_reqF_c_r__write_1__VAL_2,
	       MUX_wsiM_reqFifo_c_r__write_1__VAL_1,
	       MUX_wsiM_reqFifo_c_r__write_1__VAL_2;
  wire MUX_endOfMessage__write_1__SEL_1,
       MUX_impreciseBurst__write_1__SEL_2,
       MUX_lastMesg__write_1__SEL_2,
       MUX_mesgCount__write_1__SEL_1,
       MUX_mesgLength__write_1__SEL_2,
       MUX_wci_illegalEdge__write_1__SEL_1,
       MUX_wci_illegalEdge__write_1__SEL_2,
       MUX_wci_illegalEdge__write_1__VAL_2,
       MUX_wci_respF_q_0__write_1__SEL_1,
       MUX_wci_respF_q_1__write_1__SEL_1,
       MUX_wmi_dhF_q_0__write_1__SEL_2,
       MUX_wmi_dhF_q_1__write_1__SEL_2,
       MUX_wmi_mFlagF_q_0__write_1__SEL_2,
       MUX_wmi_mFlagF_q_1__write_1__SEL_2,
       MUX_wmi_mFlagF_x_wire__wset_1__SEL_2,
       MUX_wmi_reqF_q_0__write_1__SEL_1,
       MUX_wmi_reqF_q_1__write_1__SEL_1,
       MUX_wsiM_reqFifo_q_0__write_1__SEL_2,
       MUX_wsiM_reqFifo_q_1__write_1__SEL_2,
       MUX_wsiM_reqFifo_x_wire__wset_1__SEL_1;

  // remaining internal signals
  reg [63 : 0] v__h15314,
	       v__h16237,
	       v__h16483,
	       v__h18176,
	       v__h18253,
	       v__h19470,
	       v__h2653,
	       v__h2800,
	       v__h3699;
  reg [31 : 0] value__h6065, x_data__h15447;
  wire [31 : 0] rdat__h15540;
  wire [23 : 0] mesgMetaF_length__h16810, mesgMetaF_length__h17723;
  wire [15 : 0] wsiBurstLength__h18454, x__h15543, x_length__h17087;
  wire [13 : 0] IF_mesgLength_22_BIT_14_23_THEN_mesgLength_22__ETC___d753,
		addr__h16647,
		b__h19084,
		mlp1B__h16631,
		mlp1__h16630;
  wire [11 : 0] bl__h17580, x_burstLength__h18559;
  wire [7 : 0] mesgMetaF_opcode__h16809;
  wire [3 : 0] b__h13937, x_byteEn__h18561;
  wire NOT_wmi_reqF_c_r_46_EQ_2_47_48_AND_wmi_operate_ETC___d290,
       NOT_wsiS_reqFifo_countReg_53_ULE_1_54___d355,
       wsiS_reqFifo_i_notEmpty__52_AND_wmi_operateD_5_ETC___d165,
       x__h16715,
       x__h18884;

  // action method wciS0_mCmd
  assign CAN_FIRE_wciS0_mCmd = 1'b1 ;
  assign WILL_FIRE_wciS0_mCmd = 1'b1 ;

  // action method wciS0_mAddrSpace
  assign CAN_FIRE_wciS0_mAddrSpace = 1'b1 ;
  assign WILL_FIRE_wciS0_mAddrSpace = 1'b1 ;

  // action method wciS0_mByteEn
  assign CAN_FIRE_wciS0_mByteEn = 1'b1 ;
  assign WILL_FIRE_wciS0_mByteEn = 1'b1 ;

  // action method wciS0_mAddr
  assign CAN_FIRE_wciS0_mAddr = 1'b1 ;
  assign WILL_FIRE_wciS0_mAddr = 1'b1 ;

  // action method wciS0_mData
  assign CAN_FIRE_wciS0_mData = 1'b1 ;
  assign WILL_FIRE_wciS0_mData = 1'b1 ;

  // value method wciS0_sResp
  assign wciS0_SResp = wci_respF_q_0[33:32] ;

  // value method wciS0_sData
  assign wciS0_SData = wci_respF_q_0[31:0] ;

  // value method wciS0_sThreadBusy
  assign wciS0_SThreadBusy = wci_reqF_countReg > 2'b01 || wci_isReset__VAL ;

  // value method wciS0_sFlag
  assign wciS0_SFlag = { 1'b1, wci_sFlagReg } ;

  // action method wciS0_mFlag
  assign CAN_FIRE_wciS0_mFlag = 1'b1 ;
  assign WILL_FIRE_wciS0_mFlag = 1'b1 ;

  // value method wmiM_mCmd
  assign wmiM_MCmd = wmi_sThreadBusy_d ? 3'b000 : wmi_reqF_q_0[31:29] ;

  // value method wmiM_mReqLast
  assign wmiM_MReqLast = wmi_reqF_q_0[28] ;

  // value method wmiM_mReqInfo
  assign wmiM_MReqInfo = wmi_reqF_q_0[27] ;

  // value method wmiM_mAddrSpace
  assign wmiM_MAddrSpace = wmi_reqF_q_0[26] ;

  // value method wmiM_mAddr
  assign wmiM_MAddr = wmi_reqF_q_0[25:12] ;

  // value method wmiM_mBurstLength
  assign wmiM_MBurstLength = wmi_reqF_q_0[11:0] ;

  // value method wmiM_mDataValid
  assign wmiM_MDataValid = !wmi_sDataThreadBusy_d && wmi_dhF_q_0[37] ;

  // value method wmiM_mDataLast
  assign wmiM_MDataLast = wmi_dhF_q_0[36] ;

  // value method wmiM_mData
  assign wmiM_MData = wmi_dhF_q_0[35:4] ;

  // value method wmiM_mDataByteEn
  assign wmiM_MDataByteEn = wmi_dhF_q_0[3:0] ;

  // action method wmiM_sResp
  assign CAN_FIRE_wmiM_sResp = 1'b1 ;
  assign WILL_FIRE_wmiM_sResp = 1'b1 ;

  // action method wmiM_sData
  assign CAN_FIRE_wmiM_sData = 1'b1 ;
  assign WILL_FIRE_wmiM_sData = 1'b1 ;

  // action method wmiM_sThreadBusy
  assign CAN_FIRE_wmiM_sThreadBusy = 1'b1 ;
  assign WILL_FIRE_wmiM_sThreadBusy = wmiM_SThreadBusy ;

  // action method wmiM_sDataThreadBusy
  assign CAN_FIRE_wmiM_sDataThreadBusy = 1'b1 ;
  assign WILL_FIRE_wmiM_sDataThreadBusy = wmiM_SDataThreadBusy ;

  // action method wmiM_sRespLast
  assign CAN_FIRE_wmiM_sRespLast = 1'b1 ;
  assign WILL_FIRE_wmiM_sRespLast = wmiM_SRespLast ;

  // action method wmiM_sFlag
  assign CAN_FIRE_wmiM_sFlag = 1'b1 ;
  assign WILL_FIRE_wmiM_sFlag = 1'b1 ;

  // value method wmiM_mFlag
  assign wmiM_MFlag = wmi_sThreadBusy_d ? 32'b00000000000000000000000000000000 : wmi_mFlagF_q_0 ;

  // value method wmiM_mReset_n
  assign wmiM_MReset_n = !wmi_isReset__VAL && wmi_operateD ;

  // action method wmiM_sReset_n
  assign CAN_FIRE_wmiM_sReset_n = 1'b1 ;
  assign WILL_FIRE_wmiM_sReset_n = wmiM_SReset_n ;

  // value method wsiM1_mCmd
  assign wsiM1_MCmd = wsiM_sThreadBusy_d ? 3'b000 : wsiM_reqFifo_q_0[60:58] ;

  // value method wsiM1_mReqLast
  assign wsiM1_MReqLast = !wsiM_sThreadBusy_d && wsiM_reqFifo_q_0[57] ;

  // value method wsiM1_mBurstPrecise
  assign wsiM1_MBurstPrecise = !wsiM_sThreadBusy_d && wsiM_reqFifo_q_0[56] ;

  // value method wsiM1_mBurstLength
  assign wsiM1_MBurstLength =
	     wsiM_sThreadBusy_d ? 12'b000000000000 : wsiM_reqFifo_q_0[55:44] ;

  // value method wsiM1_mData
  assign wsiM1_MData = wsiM_reqFifo_q_0[43:12] ;

  // value method wsiM1_mByteEn
  assign wsiM1_MByteEn = wsiM_reqFifo_q_0[11:8] ;

  // value method wsiM1_mReqInfo
  assign wsiM1_MReqInfo = wsiM_sThreadBusy_d ? 8'b00000000 : wsiM_reqFifo_q_0[7:0] ;

  // action method wsiM1_sThreadBusy
  assign CAN_FIRE_wsiM1_sThreadBusy = 1'b1 ;
  assign WILL_FIRE_wsiM1_sThreadBusy = wsiM1_SThreadBusy ;

  // value method wsiM1_mReset_n
  assign wsiM1_MReset_n = !wsiM_isReset__VAL && wsiM_operateD ;

  // action method wsiM1_sReset_n
  assign CAN_FIRE_wsiM1_sReset_n = 1'b1 ;
  assign WILL_FIRE_wsiM1_sReset_n = wsiM1_SReset_n ;

  // action method wsiS1_mCmd
  assign CAN_FIRE_wsiS1_mCmd = 1'b1 ;
  assign WILL_FIRE_wsiS1_mCmd = 1'b1 ;

  // action method wsiS1_mReqLast
  assign CAN_FIRE_wsiS1_mReqLast = 1'b1 ;
  assign WILL_FIRE_wsiS1_mReqLast = wsiS1_MReqLast ;

  // action method wsiS1_mBurstPrecise
  assign CAN_FIRE_wsiS1_mBurstPrecise = 1'b1 ;
  assign WILL_FIRE_wsiS1_mBurstPrecise = wsiS1_MBurstPrecise ;

  // action method wsiS1_mBurstLength
  assign CAN_FIRE_wsiS1_mBurstLength = 1'b1 ;
  assign WILL_FIRE_wsiS1_mBurstLength = 1'b1 ;

  // action method wsiS1_mData
  assign CAN_FIRE_wsiS1_mData = 1'b1 ;
  assign WILL_FIRE_wsiS1_mData = 1'b1 ;

  // action method wsiS1_mByteEn
  assign CAN_FIRE_wsiS1_mByteEn = 1'b1 ;
  assign WILL_FIRE_wsiS1_mByteEn = 1'b1 ;

  // action method wsiS1_mReqInfo
  assign CAN_FIRE_wsiS1_mReqInfo = 1'b1 ;
  assign WILL_FIRE_wsiS1_mReqInfo = 1'b1 ;

  // action method wsiS1_mDataInfo
  assign CAN_FIRE_wsiS1_mDataInfo = 1'b1 ;
  assign WILL_FIRE_wsiS1_mDataInfo = 1'b1 ;

  // value method wsiS1_sThreadBusy
  assign wsiS1_SThreadBusy =
	     NOT_wsiS_reqFifo_countReg_53_ULE_1_54___d355 ||
	     wsiS_isReset__VAL ||
	     !wsiS_operateD ||
	     !wsiS_peerIsReady ;

  // value method wsiS1_sReset_n
  assign wsiS1_SReset_n = !wsiS_isReset__VAL && wsiS_operateD ;

  // action method wsiS1_mReset_n
  assign CAN_FIRE_wsiS1_mReset_n = 1'b1 ;
  assign WILL_FIRE_wsiS1_mReset_n = wsiS1_MReset_n ;

  // submodule wci_isReset

////
//wire full_not_used;
////
  // submodule wci_reqF
//  SizedFIFO_a size_fifoA(
//
//					.CLK(wciS0_Clk),
//					.D_IN(wci_reqF__D_IN),
//					.ENQ(wci_reqF__ENQ),
//					.DEQ(wci_reqF__DEQ),
//					.CLR(wci_reqF__CLR),
//					.D_OUT(wci_reqF__D_OUT),
//					.FULL_N(full_not_used),
//					.EMPTY_N(wci_reqF__EMPTY_N)
//					);

wire dummy1;
assign dummy1 = &wci_reqF__D_OUT;
assign prevent_sweep_node = dummy1 & dummy2 & dummy3;
////
//assign wmi_respF__CLR = 1'b0 ;
ResetToBool wmi_isReset(.RST(wciS0_MReset_n), .VAL(wmi_isReset__VAL));
ResetToBool wsiM_isReset(.RST(wciS0_MReset_n), .VAL(wsiM_isReset__VAL));
ResetToBool wsiS_isReset(.RST(wciS0_MReset_n), .VAL(wsiS_isReset__VAL));
ResetToBool wci_isReset(.RST(wciS0_MReset_n), .VAL(wci_isReset__VAL));
////
  // submodule wmi_isReset
  

  // submodule wmi_respF
  //34 width
////
//  SizedFIFO_b size_fifoB 	(
//						     .CLK(wciS0_Clk),
//						     .D_IN(wmi_respF__D_IN),
//						     .ENQ(wmi_respF__ENQ),
//						     .DEQ(wmi_respF__DEQ),
//						     .CLR(wmi_respF__CLR),
//						     .D_OUT(wmi_respF__D_OUT),
//						     .FULL_N(wmi_respF__FULL_N),
//						     .EMPTY_N(wmi_respF__EMPTY_N)
//							 );
////
wire dummy2;
assign dummy2 = &wmi_respF__D_OUT;

  // submodule wsiM_isReset
  

  // submodule wsiS_isReset
  

  // submodule wsiS_reqFifo
 // SizedFIFO #(.p1width(32'd61),
//	      .p2depth(32'd3),
	//      .p3cntr_width(32'b00000000000000000000000000000001),
	//      .guarded(32'b00000000000000000000000000000001)) wsiS_reqFifo(.RST_N(wciS0_MReset_n),

////
//				SizedFIFO_c size_fifoc(
//
//					    .CLK(wciS0_Clk),
//					    .D_IN(wsiS_reqFifo__D_IN),
//					    .ENQ(wsiS_reqFifo__ENQ),
//					    .DEQ(wsiS_reqFifo__DEQ),
//					    .CLR(wsiS_reqFifo__CLR),
//					    .D_OUT(wsiS_reqFifo__D_OUT),
//					    .FULL_N(wsiS_reqFifo__FULL_N),
//					    .EMPTY_N(wsiS_reqFifo__EMPTY_N)
//						);
////

wire dummy3;
assign dummy3 = &wsiS_reqFifo__D_OUT;


  // rule RL_wci_request_decode
  assign CAN_FIRE_RL_wci_request_decode = wci_reqF__EMPTY_N ;
  assign WILL_FIRE_RL_wci_request_decode = wci_reqF__EMPTY_N ;

  // rule RL_wsiS_ext_status_assign
  assign CAN_FIRE_RL_wsiS_ext_status_assign = 1'b1 ;
  assign WILL_FIRE_RL_wsiS_ext_status_assign = 1'b1 ;

  // rule RL_wsiM_ext_status_assign
  assign CAN_FIRE_RL_wsiM_ext_status_assign = 1'b1 ;
  assign WILL_FIRE_RL_wsiM_ext_status_assign = 1'b1 ;

  // rule RL_wci_cfrd
  assign CAN_FIRE_RL_wci_cfrd =
	     wci_respF_c_r != 2'b10 && wci_reqF__EMPTY_N &&
	     wci_wci_cfrd_pw__whas ;
  assign WILL_FIRE_RL_wci_cfrd =
	     CAN_FIRE_RL_wci_cfrd && !WILL_FIRE_RL_wci_ctl_op_start &&
	     !WILL_FIRE_RL_wci_ctl_op_complete ;

  // rule RL_wmrd_mesgBodyRequest
  assign CAN_FIRE_RL_wmrd_mesgBodyRequest =
	     NOT_wmi_reqF_c_r_46_EQ_2_47_48_AND_wmi_operate_ETC___d290 &&
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h1 || smaCtrl[3:0] == 4'h4 ||
	      smaCtrl[3:0] == 4'h9) &&
	     mesgPreRequest ;
  assign WILL_FIRE_RL_wmrd_mesgBodyRequest =
	     CAN_FIRE_RL_wmrd_mesgBodyRequest ;

  // rule RL_wmrd_mesgBodyPreRequest
  assign CAN_FIRE_RL_wmrd_mesgBodyPreRequest =
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h1 || smaCtrl[3:0] == 4'h4 ||
	      smaCtrl[3:0] == 4'h9) &&
	     fabWordsRemain != 14'b00000000000000 &&
	     (fabRespCredit_value ^ 4'h8) > 4'b1000 &&
	     mesgReqOK ;
  assign WILL_FIRE_RL_wmrd_mesgBodyPreRequest =
	     CAN_FIRE_RL_wmrd_mesgBodyPreRequest &&
	     !WILL_FIRE_RL_wmrd_mesgBodyRequest ;

  // rule RL_wmrd_mesgBegin
  assign CAN_FIRE_RL_wmrd_mesgBegin =
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h1 || smaCtrl[3:0] == 4'h4 ||
	      smaCtrl[3:0] == 4'h9) &&
	     !wmi_sThreadBusy_d &&
	     !wmi_sDataThreadBusy_d &&
	     unrollCnt == 16'b0000000000000000 ;
  assign WILL_FIRE_RL_wmrd_mesgBegin = CAN_FIRE_RL_wmrd_mesgBegin ;

  // rule RL_wci_ctl_op_start
  assign CAN_FIRE_RL_wci_ctl_op_start =
	     wci_reqF__EMPTY_N && wci_wci_ctrl_pw__whas ;
  assign WILL_FIRE_RL_wci_ctl_op_start =
	     CAN_FIRE_RL_wci_ctl_op_start &&
	     !WILL_FIRE_RL_wci_ctl_op_complete ;

  // rule RL_wci_ctrl_EiI
  assign CAN_FIRE_RL_wci_ctrl_EiI =
	     wci_wci_ctrl_pw__whas && WILL_FIRE_RL_wci_ctl_op_start &&
	     wci_cState == 3'b000 &&
	     wci_reqF__D_OUT[36:34] == 3'b000 ;
  assign WILL_FIRE_RL_wci_ctrl_EiI = CAN_FIRE_RL_wci_ctrl_EiI ;

  // rule RL_wci_ctrl_OrE
  assign CAN_FIRE_RL_wci_ctrl_OrE =
	     wci_wci_ctrl_pw__whas && WILL_FIRE_RL_wci_ctl_op_start &&
	     wci_cState == 3'b010 &&
	     wci_reqF__D_OUT[36:34] == 3'b011 ;
  assign WILL_FIRE_RL_wci_ctrl_OrE = CAN_FIRE_RL_wci_ctrl_OrE ;

  // rule RL_wci_respF_deq
  assign CAN_FIRE_RL_wci_respF_deq = 1'b1 ;
  assign WILL_FIRE_RL_wci_respF_deq = 1'b1 ;

  // rule RL_wci_sThreadBusy_reg
  assign CAN_FIRE_RL_wci_sThreadBusy_reg = 1'b1 ;
  assign WILL_FIRE_RL_wci_sThreadBusy_reg = 1'b1 ;

  // rule RL_wci_sFlagReg__dreg_update
  assign CAN_FIRE_RL_wci_sFlagReg__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wci_sFlagReg__dreg_update = 1'b1 ;

  // rule RL_wsi_Es_doAlways
  assign CAN_FIRE_RL_wsi_Es_doAlways = 1'b1 ;
  assign WILL_FIRE_RL_wsi_Es_doAlways = 1'b1 ;

  // rule RL_wsiS_update_statusR
  assign CAN_FIRE_RL_wsiS_update_statusR = 1'b1 ;
  assign WILL_FIRE_RL_wsiS_update_statusR = 1'b1 ;

  // rule RL_wsiS_inc_tBusyCount
  assign CAN_FIRE_RL_wsiS_inc_tBusyCount =
	     wsiS_operateD && wsiS_peerIsReady &&
	     NOT_wsiS_reqFifo_countReg_53_ULE_1_54___d355 ;
  assign WILL_FIRE_RL_wsiS_inc_tBusyCount = CAN_FIRE_RL_wsiS_inc_tBusyCount ;

  // rule RL_wsiS_reqFifo_enq
  assign CAN_FIRE_RL_wsiS_reqFifo_enq =
	     wsiS_operateD && wsiS_peerIsReady &&
	     wsiS1_MCmd[2:0] == 3'b001 ;
  assign WILL_FIRE_RL_wsiS_reqFifo_enq = CAN_FIRE_RL_wsiS_reqFifo_enq ;

  // rule RL_wsiS_peerIsReady__dreg_update
  assign CAN_FIRE_RL_wsiS_peerIsReady__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wsiS_peerIsReady__dreg_update = 1'b1 ;

  // rule RL_wsiM_update_statusR
  assign CAN_FIRE_RL_wsiM_update_statusR = 1'b1 ;
  assign WILL_FIRE_RL_wsiM_update_statusR = 1'b1 ;

  // rule RL_wsiM_inc_tBusyCount
  assign CAN_FIRE_RL_wsiM_inc_tBusyCount =
	     wsiM_operateD && wsiM_peerIsReady && wsiM_sThreadBusy_d ;
  assign WILL_FIRE_RL_wsiM_inc_tBusyCount = CAN_FIRE_RL_wsiM_inc_tBusyCount ;

  // rule RL_wsiM_reqFifo_deq
  assign CAN_FIRE_RL_wsiM_reqFifo_deq =
	     wsiM_reqFifo_c_r != 2'b00 && !wsiM_sThreadBusy_d ;
  assign WILL_FIRE_RL_wsiM_reqFifo_deq = CAN_FIRE_RL_wsiM_reqFifo_deq ;

  // rule RL_wsiM_sThreadBusy_reg
  assign CAN_FIRE_RL_wsiM_sThreadBusy_reg = 1'b1 ;
  assign WILL_FIRE_RL_wsiM_sThreadBusy_reg = 1'b1 ;

  // rule RL_wsiM_peerIsReady__dreg_update
  assign CAN_FIRE_RL_wsiM_peerIsReady__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wsiM_peerIsReady__dreg_update = 1'b1 ;

  // rule RL_operating_actions
  assign CAN_FIRE_RL_operating_actions = wci_cState == 3'b010 ;
  assign WILL_FIRE_RL_operating_actions = wci_cState == 3'b010 ;

  // rule RL_wsiS_operateD__dreg_update
  assign CAN_FIRE_RL_wsiS_operateD__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wsiS_operateD__dreg_update = 1'b1 ;

  // rule RL_wsiM_operateD__dreg_update
  assign CAN_FIRE_RL_wsiM_operateD__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wsiM_operateD__dreg_update = 1'b1 ;

  // rule RL_wmrd_mesgBodyResponse
  assign CAN_FIRE_RL_wmrd_mesgBodyResponse =
	     wmi_respF__EMPTY_N && (smaCtrl[4] || wsiM_reqFifo_c_r != 2'b10) &&
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h1 || smaCtrl[3:0] == 4'h4 ||
	      smaCtrl[3:0] == 4'h9) &&
	     unrollCnt != 16'b0000000000000000 ;
  assign WILL_FIRE_RL_wmrd_mesgBodyResponse =
	     CAN_FIRE_RL_wmrd_mesgBodyResponse ;

  // rule RL_wmwt_messagePushImprecise
  assign CAN_FIRE_RL_wmwt_messagePushImprecise =
	     wmi_reqF_c_r != 2'b10 && wmi_dhF_c_r != 2'b10 &&
	     wsiS_reqFifo_i_notEmpty__52_AND_wmi_operateD_5_ETC___d165 &&
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h2 || smaCtrl[3:0] == 4'h3) &&
	     readyToPush &&
	     impreciseBurst ;
  assign WILL_FIRE_RL_wmwt_messagePushImprecise =
	     CAN_FIRE_RL_wmwt_messagePushImprecise &&
	     !WILL_FIRE_RL_wmwt_messageFinalize ;

  // rule RL_wmwt_messagePushPrecise
  assign CAN_FIRE_RL_wmwt_messagePushPrecise =
	     wmi_dhF_c_r != 2'b10 && wsiS_reqFifo__EMPTY_N && wmi_operateD &&
	     wmi_peerIsReady &&
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h2 || smaCtrl[3:0] == 4'h3) &&
	     wsiWordsRemain != 12'b000000000000 &&
	     mesgReqValid &&
	     preciseBurst ;
  assign WILL_FIRE_RL_wmwt_messagePushPrecise =
	     CAN_FIRE_RL_wmwt_messagePushPrecise &&
	     !WILL_FIRE_RL_wmwt_messagePushImprecise &&
	     !WILL_FIRE_RL_wmwt_messageFinalize ;

  // rule RL_wmwt_requestPrecise
  assign CAN_FIRE_RL_wmwt_requestPrecise =
	     wmi_reqF_c_r != 2'b10 && wmi_mFlagF_c_r != 2'b10 && wmi_operateD &&
	     wmi_peerIsReady &&
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h2 || smaCtrl[3:0] == 4'h3) &&
	     readyToRequest &&
	     preciseBurst ;
  assign WILL_FIRE_RL_wmwt_requestPrecise =
	     CAN_FIRE_RL_wmwt_requestPrecise &&
	     !WILL_FIRE_RL_wmwt_messagePushImprecise ;

  // rule RL_wmwt_messageFinalize
  assign CAN_FIRE_RL_wmwt_messageFinalize =
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h2 || smaCtrl[3:0] == 4'h3) &&
	     mesgLength[14] &&
	     !doAbort &&
	     (preciseBurst && wsiWordsRemain == 12'b000000000000 ||
	      impreciseBurst && endOfMessage) ;
  assign WILL_FIRE_RL_wmwt_messageFinalize =
	     CAN_FIRE_RL_wmwt_messageFinalize ;

  // rule RL_wmwt_mesgBegin
  assign CAN_FIRE_RL_wmwt_mesgBegin =
	     wsiS_reqFifo__EMPTY_N && wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h2 || smaCtrl[3:0] == 4'h3) &&
	     !wmi_sThreadBusy_d &&
	     !wmi_sDataThreadBusy_d &&
	     !opcode[8] ;
  assign WILL_FIRE_RL_wmwt_mesgBegin =
	     CAN_FIRE_RL_wmwt_mesgBegin &&
	     !WILL_FIRE_RL_wmwt_messagePushPrecise &&
	     !WILL_FIRE_RL_wmwt_messagePushImprecise &&
	     !WILL_FIRE_RL_wmwt_messageFinalize ;

  // rule RL_wmwt_doAbort
  assign CAN_FIRE_RL_wmwt_doAbort =
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h2 || smaCtrl[3:0] == 4'h3) &&
	     doAbort ;
  assign WILL_FIRE_RL_wmwt_doAbort = CAN_FIRE_RL_wmwt_doAbort ;

  // rule RL_wmi_Em_doAlways
  assign CAN_FIRE_RL_wmi_Em_doAlways = 1'b1 ;
  assign WILL_FIRE_RL_wmi_Em_doAlways = 1'b1 ;

  // rule RL_wci_Es_doAlways_Req
  assign CAN_FIRE_RL_wci_Es_doAlways_Req = 1'b1 ;
  assign WILL_FIRE_RL_wci_Es_doAlways_Req = 1'b1 ;

  // rule RL_wci_reqF_enq
////  
//  assign CAN_FIRE_RL_wci_reqF_enq = wciS0_MCmd[2:0] != 3'b000 ;
//  assign WILL_FIRE_RL_wci_reqF_enq = CAN_FIRE_RL_wci_reqF_enq ;
////
  // rule RL_wsipass_doMessagePush
  assign CAN_FIRE_RL_wsipass_doMessagePush =
	     wsiS_reqFifo__EMPTY_N &&
	     (smaCtrl[4] || wsiM_reqFifo_c_r != 2'b10) &&
	     wci_cState == 3'b010 &&
	     (smaCtrl[3:0] == 4'h0 || smaCtrl[3:0] == 4'h3) ;
  assign WILL_FIRE_RL_wsipass_doMessagePush =
	     CAN_FIRE_RL_wsipass_doMessagePush &&
	     !WILL_FIRE_RL_wmwt_messagePushPrecise &&
	     !WILL_FIRE_RL_wmwt_messagePushImprecise ;

  // rule RL_wci_cfwr
  assign CAN_FIRE_RL_wci_cfwr =
	     wci_respF_c_r != 2'b10 && wci_reqF__EMPTY_N &&
	     wci_wci_cfwr_pw__whas ;
  assign WILL_FIRE_RL_wci_cfwr =
	     CAN_FIRE_RL_wci_cfwr && !WILL_FIRE_RL_wci_ctl_op_start &&
	     !WILL_FIRE_RL_wci_ctl_op_complete ;

  // rule RL_wci_reqF__updateLevelCounter
  assign CAN_FIRE_RL_wci_reqF__updateLevelCounter =
	     (wciS0_MCmd[2:0] != 3'b000) != wci_reqF_r_deq__whas ;
  assign WILL_FIRE_RL_wci_reqF__updateLevelCounter =
	     CAN_FIRE_RL_wci_reqF__updateLevelCounter ;

  // rule RL_wsiS_reqFifo__updateLevelCounter
  assign CAN_FIRE_RL_wsiS_reqFifo__updateLevelCounter =
	     CAN_FIRE_RL_wsiS_reqFifo_enq != wsiS_reqFifo_r_deq__whas ;
  assign WILL_FIRE_RL_wsiS_reqFifo__updateLevelCounter =
	     CAN_FIRE_RL_wsiS_reqFifo__updateLevelCounter ;

  // rule RL_wsiM_reqFifo_both
  assign CAN_FIRE_RL_wsiM_reqFifo_both =
	     ((wsiM_reqFifo_c_r == 2'b01) ?
		wsiM_reqFifo_x_wire__whas :
		wsiM_reqFifo_c_r != 2'b10 || wsiM_reqFifo_x_wire__whas) &&
	     CAN_FIRE_RL_wsiM_reqFifo_deq &&
	     wsiM_reqFifo_enqueueing__whas ;
  assign WILL_FIRE_RL_wsiM_reqFifo_both = CAN_FIRE_RL_wsiM_reqFifo_both ;

  // rule RL_wsiM_reqFifo_decCtr
  assign CAN_FIRE_RL_wsiM_reqFifo_decCtr =
	     CAN_FIRE_RL_wsiM_reqFifo_deq && !wsiM_reqFifo_enqueueing__whas ;
  assign WILL_FIRE_RL_wsiM_reqFifo_decCtr = CAN_FIRE_RL_wsiM_reqFifo_decCtr ;

  // rule RL_wsiM_reqFifo_incCtr
  assign CAN_FIRE_RL_wsiM_reqFifo_incCtr =
	     ((wsiM_reqFifo_c_r == 2'b00) ?
		wsiM_reqFifo_x_wire__whas :
		wsiM_reqFifo_c_r != 2'b01 || wsiM_reqFifo_x_wire__whas) &&
	     wsiM_reqFifo_enqueueing__whas &&
	     !CAN_FIRE_RL_wsiM_reqFifo_deq ;
  assign WILL_FIRE_RL_wsiM_reqFifo_incCtr = CAN_FIRE_RL_wsiM_reqFifo_incCtr ;

  // rule RL_mesgPreRequest__dreg_update
  assign CAN_FIRE_RL_mesgPreRequest__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_mesgPreRequest__dreg_update = 1'b1 ;

  // rule RL_fabRespCredit_accumulate
  assign CAN_FIRE_RL_fabRespCredit_accumulate = 1'b1 ;
  assign WILL_FIRE_RL_fabRespCredit_accumulate = 1'b1 ;

  // rule RL_wci_ctrl_IsO
  assign CAN_FIRE_RL_wci_ctrl_IsO =
	     wci_wci_ctrl_pw__whas && WILL_FIRE_RL_wci_ctl_op_start &&
	     wci_cState == 3'b001 &&
	     wci_reqF__D_OUT[36:34] == 3'b001 ;
  assign WILL_FIRE_RL_wci_ctrl_IsO = CAN_FIRE_RL_wci_ctrl_IsO ;

  // rule RL_wci_ctl_op_complete
  assign CAN_FIRE_RL_wci_ctl_op_complete =
	     wci_respF_c_r != 2'b10 && wci_ctlOpActive && wci_ctlAckReg ;
  assign WILL_FIRE_RL_wci_ctl_op_complete = CAN_FIRE_RL_wci_ctl_op_complete ;

  // rule RL_wci_ctlAckReg__dreg_update
  assign CAN_FIRE_RL_wci_ctlAckReg__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wci_ctlAckReg__dreg_update = 1'b1 ;

  // rule RL_wci_respF_both
  assign CAN_FIRE_RL_wci_respF_both =
	     ((wci_respF_c_r == 2'b01) ?
		wci_respF_x_wire__whas :
		wci_respF_c_r != 2'b10 || wci_respF_x_wire__whas) &&
	     wci_respF_c_r != 2'b00 &&
	     wci_respF_enqueueing__whas ;
  assign WILL_FIRE_RL_wci_respF_both = CAN_FIRE_RL_wci_respF_both ;

  // rule RL_wci_respF_decCtr
  assign CAN_FIRE_RL_wci_respF_decCtr =
	     wci_respF_c_r != 2'b00 && !wci_respF_enqueueing__whas ;
  assign WILL_FIRE_RL_wci_respF_decCtr = CAN_FIRE_RL_wci_respF_decCtr ;

  // rule RL_wci_respF_incCtr
  assign CAN_FIRE_RL_wci_respF_incCtr =
	     ((wci_respF_c_r == 2'b00) ?
		wci_respF_x_wire__whas :
		wci_respF_c_r != 2'b01 || wci_respF_x_wire__whas) &&
	     wci_respF_enqueueing__whas &&
	     !(wci_respF_c_r != 2'b00) ;
  assign WILL_FIRE_RL_wci_respF_incCtr = CAN_FIRE_RL_wci_respF_incCtr ;

  // rule RL_wmi_dhF_deq
  assign CAN_FIRE_RL_wmi_dhF_deq = !wmi_sDataThreadBusy_d ;
  assign WILL_FIRE_RL_wmi_dhF_deq = CAN_FIRE_RL_wmi_dhF_deq ;

  // rule RL_wmi_respAdvance
  assign CAN_FIRE_RL_wmi_respAdvance =
	     wmi_respF__FULL_N && wmi_operateD && wmi_peerIsReady &&
	     wmiM_SResp[1:0] != 2'b00 ;
  assign WILL_FIRE_RL_wmi_respAdvance = CAN_FIRE_RL_wmi_respAdvance ;

  // rule RL_wmi_reqF_deq
  assign CAN_FIRE_RL_wmi_reqF_deq = !wmi_sThreadBusy_d ;
  assign WILL_FIRE_RL_wmi_reqF_deq = CAN_FIRE_RL_wmi_reqF_deq ;

  // rule RL_wmi_peerIsReady__dreg_update
  assign CAN_FIRE_RL_wmi_peerIsReady__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wmi_peerIsReady__dreg_update = 1'b1 ;

  // rule RL_wmi_operateD__dreg_update
  assign CAN_FIRE_RL_wmi_operateD__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wmi_operateD__dreg_update = 1'b1 ;

  // rule RL_wmi_sDataThreadBusy_d__dreg_update
  assign CAN_FIRE_RL_wmi_sDataThreadBusy_d__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wmi_sDataThreadBusy_d__dreg_update = 1'b1 ;

  // rule RL_wmi_sThreadBusy_d__dreg_update
  assign CAN_FIRE_RL_wmi_sThreadBusy_d__dreg_update = 1'b1 ;
  assign WILL_FIRE_RL_wmi_sThreadBusy_d__dreg_update = 1'b1 ;

  // rule RL_wmi_dhF_decCtr
  assign CAN_FIRE_RL_wmi_dhF_decCtr =
	     wmi_dhF_dequeueing__whas && !wmi_dhF_enqueueing__whas ;
  assign WILL_FIRE_RL_wmi_dhF_decCtr = CAN_FIRE_RL_wmi_dhF_decCtr ;

  // rule RL_wmi_dhF_both
  assign CAN_FIRE_RL_wmi_dhF_both =
	     ((wmi_dhF_c_r == 2'b01) ?
		wmi_dhF_enqueueing__whas :
		wmi_dhF_c_r != 2'b10 || wmi_dhF_enqueueing__whas) &&
	     wmi_dhF_dequeueing__whas &&
	     wmi_dhF_enqueueing__whas ;
  assign WILL_FIRE_RL_wmi_dhF_both = CAN_FIRE_RL_wmi_dhF_both ;

  // rule RL_wmi_dhF_incCtr
  assign CAN_FIRE_RL_wmi_dhF_incCtr =
	     ((wmi_dhF_c_r == 2'b00) ?
		wmi_dhF_enqueueing__whas :
		wmi_dhF_c_r != 2'b01 || wmi_dhF_enqueueing__whas) &&
	     wmi_dhF_enqueueing__whas &&
	     !wmi_dhF_dequeueing__whas ;
  assign WILL_FIRE_RL_wmi_dhF_incCtr = CAN_FIRE_RL_wmi_dhF_incCtr ;

  // rule RL_wmi_mFlagF_both
  assign CAN_FIRE_RL_wmi_mFlagF_both =
	     ((wmi_mFlagF_c_r == 2'b01) ?
		wmi_mFlagF_enqueueing__whas :
		wmi_mFlagF_c_r != 2'b10 || wmi_mFlagF_enqueueing__whas) &&
	     wmi_mFlagF_dequeueing__whas &&
	     wmi_mFlagF_enqueueing__whas ;
  assign WILL_FIRE_RL_wmi_mFlagF_both = CAN_FIRE_RL_wmi_mFlagF_both ;

  // rule RL_wmi_mFlagF_decCtr
  assign CAN_FIRE_RL_wmi_mFlagF_decCtr =
	     wmi_mFlagF_dequeueing__whas && !wmi_mFlagF_enqueueing__whas ;
  assign WILL_FIRE_RL_wmi_mFlagF_decCtr = CAN_FIRE_RL_wmi_mFlagF_decCtr ;

  // rule RL_wmi_mFlagF_incCtr
  assign CAN_FIRE_RL_wmi_mFlagF_incCtr =
	     ((wmi_mFlagF_c_r == 2'b00) ?
		wmi_mFlagF_enqueueing__whas :
		wmi_mFlagF_c_r != 2'b01 || wmi_mFlagF_enqueueing__whas) &&
	     wmi_mFlagF_enqueueing__whas &&
	     !wmi_mFlagF_dequeueing__whas ;
  assign WILL_FIRE_RL_wmi_mFlagF_incCtr = CAN_FIRE_RL_wmi_mFlagF_incCtr ;

  // rule RL_wmi_reqF_both
  assign CAN_FIRE_RL_wmi_reqF_both =
	     ((wmi_reqF_c_r == 2'b01) ?
		wmi_reqF_x_wire__whas :
		wmi_reqF_c_r != 2'b10 || wmi_reqF_x_wire__whas) &&
	     wmi_reqF_dequeueing__whas &&
	     wmi_reqF_enqueueing__whas ;
  assign WILL_FIRE_RL_wmi_reqF_both = CAN_FIRE_RL_wmi_reqF_both ;

  // rule RL_wmi_reqF_incCtr
  assign CAN_FIRE_RL_wmi_reqF_incCtr =
	     ((wmi_reqF_c_r == 2'b00) ?
		wmi_reqF_x_wire__whas :
		wmi_reqF_c_r != 2'b01 || wmi_reqF_x_wire__whas) &&
	     wmi_reqF_enqueueing__whas &&
	     !wmi_reqF_dequeueing__whas ;
  assign WILL_FIRE_RL_wmi_reqF_incCtr = CAN_FIRE_RL_wmi_reqF_incCtr ;

  // rule RL_wmi_reqF_decCtr
  assign CAN_FIRE_RL_wmi_reqF_decCtr =
	     wmi_reqF_dequeueing__whas && !wmi_reqF_enqueueing__whas ;
  assign WILL_FIRE_RL_wmi_reqF_decCtr = CAN_FIRE_RL_wmi_reqF_decCtr ;

  // inputs to muxes for submodule ports
  assign MUX_wci_illegalEdge__write_1__SEL_1 =
	     WILL_FIRE_RL_wci_ctl_op_complete && wci_illegalEdge ;
  assign MUX_wci_illegalEdge__write_1__VAL_2 =
	     wci_reqF__D_OUT[36:34] != 3'b100 && wci_reqF__D_OUT[36:34] != 3'b101 &&
	     wci_reqF__D_OUT[36:34] != 3'b110 ;
  assign MUX_wci_respF_c_r__write_1__VAL_1 = wci_respF_c_r - 2'b01 ;
  assign MUX_wci_respF_c_r__write_1__VAL_2 = wci_respF_c_r + 2'b01 ;
  assign MUX_wci_respF_x_wire__wset_1__VAL_1 =
	     wci_illegalEdge ? 34'h3C0DE4202 : 34'h1C0DE4201 ;
  assign MUX_wci_illegalEdge__write_1__SEL_2 =
	     WILL_FIRE_RL_wci_ctl_op_start &&
	     (wci_reqF__D_OUT[36:34] == 3'b000 && wci_cState != 3'b000 ||
	      wci_reqF__D_OUT[36:34] == 3'b001 && wci_cState != 3'b001 &&
	      wci_cState != 3'b011 ||
	      wci_reqF__D_OUT[36:34] == 3'b010 && wci_cState != 3'b010 ||
	      wci_reqF__D_OUT[36:34] == 3'b011 && wci_cState != 3'b011 &&
	      wci_cState != 3'b010 &&
	      wci_cState != 3'b001 ||
	      wci_reqF__D_OUT[36:34] == 3'b100 ||
	      wci_reqF__D_OUT[36:34] == 3'b101 ||
	      wci_reqF__D_OUT[36:34] == 3'b110 ||
	      wci_reqF__D_OUT[36:34] == 3'b111) ;
  assign MUX_wci_respF_q_0__write_1__SEL_1 =
	     WILL_FIRE_RL_wci_respF_incCtr && wci_respF_c_r == 2'b00 ;
  assign MUX_wci_respF_q_1__write_1__SEL_1 =
	     WILL_FIRE_RL_wci_respF_incCtr && wci_respF_c_r == 2'b01 ;
  assign MUX_wsiM_reqFifo_c_r__write_1__VAL_1 = wsiM_reqFifo_c_r - 2'b01 ;
  assign MUX_wsiM_reqFifo_c_r__write_1__VAL_2 = wsiM_reqFifo_c_r + 2'b01 ;
  assign MUX_wci_respF_x_wire__wset_1__VAL_2 = { 2'b01, x_data__h15447 } ;
  always@(WILL_FIRE_RL_wci_ctl_op_complete or
	  MUX_wci_respF_x_wire__wset_1__VAL_1 or
	  WILL_FIRE_RL_wci_cfrd or
	  MUX_wci_respF_x_wire__wset_1__VAL_2 or WILL_FIRE_RL_wci_cfwr)
  begin
    case (1'b1) // synopsys parallel_case
      WILL_FIRE_RL_wci_ctl_op_complete:
	  MUX_wci_respF_q_0__write_1__VAL_1 =
	      MUX_wci_respF_x_wire__wset_1__VAL_1;
      WILL_FIRE_RL_wci_cfrd:
	  MUX_wci_respF_q_0__write_1__VAL_1 =
	      MUX_wci_respF_x_wire__wset_1__VAL_2;
      WILL_FIRE_RL_wci_cfwr: MUX_wci_respF_q_0__write_1__VAL_1 = 34'h1C0DE4201;
      default: MUX_wci_respF_q_0__write_1__VAL_1 =
		   34'h2AAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign MUX_wci_respF_q_0__write_1__VAL_2 =
	     (wci_respF_c_r == 2'b01) ?
	       MUX_wci_respF_q_0__write_1__VAL_1 :
	       wci_respF_q_1 ;
  assign MUX_wci_respF_q_1__write_1__VAL_2 =
	     (wci_respF_c_r == 2'b10) ?
	       MUX_wci_respF_q_0__write_1__VAL_1 :
	       34'h0AAAAAAAA ;
  assign MUX_wsiM_reqFifo_q_0__write_1__SEL_2 =
	     WILL_FIRE_RL_wsiM_reqFifo_incCtr && wsiM_reqFifo_c_r == 2'b00 ;
  assign MUX_wsiM_reqFifo_q_1__write_1__SEL_2 =
	     WILL_FIRE_RL_wsiM_reqFifo_incCtr && wsiM_reqFifo_c_r == 2'b01 ;
  assign MUX_endOfMessage__write_1__SEL_1 =
	     WILL_FIRE_RL_wmwt_messagePushImprecise && x__h16715 ;
  assign MUX_impreciseBurst__write_1__SEL_2 =
	     WILL_FIRE_RL_wmwt_mesgBegin && !wsiS_reqFifo__D_OUT[56] ;
  assign MUX_lastMesg__write_1__SEL_2 =
	     WILL_FIRE_RL_wmwt_requestPrecise || WILL_FIRE_RL_wmrd_mesgBegin ;
  assign MUX_mesgCount__write_1__SEL_1 =
	     WILL_FIRE_RL_wmrd_mesgBodyResponse && unrollCnt == 16'b0000000000000001 ;
  assign MUX_mesgLength__write_1__SEL_2 =
	     WILL_FIRE_RL_wmwt_mesgBegin && wsiS_reqFifo__D_OUT[56] ;
  assign MUX_wmi_mFlagF_x_wire__wset_1__SEL_2 =
	     WILL_FIRE_RL_wmrd_mesgBodyRequest && x__h18884 ;
  assign MUX_wsiM_reqFifo_x_wire__wset_1__SEL_1 =
	     WILL_FIRE_RL_wmrd_mesgBodyResponse && !smaCtrl[4] ;
  assign MUX_fabRespCredit_value__write_1__VAL_2 =
	     fabRespCredit_value +
	     (CAN_FIRE_RL_wmrd_mesgBodyRequest ? b__h13937 : 4'b0000) +
	     (CAN_FIRE_RL_wmrd_mesgBodyResponse ? 4'b0001 : 4'b0000) ;
  assign MUX_fabWordsRemain__write_1__VAL_1 =
	     (wmi_sFlagReg[23:0] == 24'b000000000000000000000000) ? 14'b00000000000001 : wmi_sFlagReg[15:2] ;
  assign MUX_fabWordsRemain__write_1__VAL_2 = fabWordsRemain - fabWordsCurReq ;
  assign MUX_mesgCount__write_1__VAL_1 = mesgCount + 32'b00000000000000000000000000000001 ;
  assign MUX_mesgLength__write_1__VAL_2 =
	     (wsiS_reqFifo__D_OUT[11:8] == 4'b0000) ?
	       15'b100000000000000 :
	       { 1'b1, wsiS_reqFifo__D_OUT[55:44], 2'b00 } ;
  assign MUX_mesgLength__write_1__VAL_4 = { 1'b1, mlp1B__h16631 } ;
  assign MUX_mesgReqAddr__write_1__VAL_2 =
	     mesgReqAddr + { fabWordsCurReq[11:0], 2'b00 } ;
  assign MUX_opcode__write_1__VAL_2 = { 1'b1, wsiS_reqFifo__D_OUT[7:0] } ;
  assign MUX_thisMesg__write_1__VAL_1 =
	     { mesgCount[7:0], mesgMetaF_opcode__h16809, x_length__h17087 } ;
  assign MUX_thisMesg__write_1__VAL_2 =
	     { mesgCount[7:0], wmi_sFlagReg[31:24], wmi_sFlagReg[15:0] } ;
  assign MUX_unrollCnt__write_1__VAL_2 = unrollCnt - 16'b0000000000000001 ;
  assign MUX_unrollCnt__write_1__VAL_1 =
	     (wmi_sFlagReg[23:0] == 24'b000000000000000000000000) ? 16'b0000000000000001 : wmi_sFlagReg[17:2] ;
  assign MUX_wmi_dhF_c_r__write_1__VAL_1 = wmi_dhF_c_r - 2'b01 ;
  assign MUX_wmi_dhF_c_r__write_1__VAL_2 = wmi_dhF_c_r + 2'b01 ;
  assign MUX_wmi_dhF_x_wire__wset_1__VAL_1 =
	     { 1'b1,
	       wsiWordsRemain == 12'b000000000001,
	       wsiS_reqFifo__D_OUT[43:12],
	       4'b1111 } ;
  assign MUX_wmi_dhF_x_wire__wset_1__VAL_2 =
	     { 1'b1, x__h16715, wsiS_reqFifo__D_OUT[43:12], 4'b1111 } ;
  assign MUX_wmi_dhF_q_0__write_1__VAL_2 =
	     WILL_FIRE_RL_wmwt_messagePushPrecise ?
	       MUX_wmi_dhF_x_wire__wset_1__VAL_1 :
	       MUX_wmi_dhF_x_wire__wset_1__VAL_2 ;
  assign MUX_wmi_dhF_q_0__write_1__VAL_1 =
	     (wmi_dhF_c_r == 2'b01) ?
	       MUX_wmi_dhF_q_0__write_1__VAL_2 :
	       wmi_dhF_q_1 ;
  assign MUX_wmi_dhF_q_1__write_1__VAL_1 =
	     (wmi_dhF_c_r == 2'b10) ?
	       MUX_wmi_dhF_q_0__write_1__VAL_2 :
	       38'h0AAAAAAAAA ;
  assign MUX_wmi_mFlagF_c_r__write_1__VAL_1 = wmi_mFlagF_c_r - 2'b01 ;
  assign MUX_wmi_mFlagF_c_r__write_1__VAL_2 = wmi_mFlagF_c_r + 2'b01 ;
  assign MUX_wmi_mFlagF_x_wire__wset_1__VAL_1 =
	     { mesgMetaF_opcode__h16809, mesgMetaF_length__h16810 } ;
  assign MUX_wmi_mFlagF_x_wire__wset_1__VAL_3 =
	     { mesgMetaF_opcode__h16809, mesgMetaF_length__h17723 } ;
  assign MUX_wmi_mFlagF_q_0__write_1__VAL_1 =
	     (wmi_mFlagF_c_r == 2'b01) ? value__h6065 : wmi_mFlagF_q_1 ;
  assign MUX_wmi_mFlagF_q_1__write_1__VAL_1 =
	     (wmi_mFlagF_c_r == 2'b10) ? value__h6065 : 32'b00000000000000000000000000000000 ;
  assign MUX_wmi_reqF_c_r__write_1__VAL_1 = wmi_reqF_c_r - 2'b01 ;
  assign MUX_wmi_reqF_c_r__write_1__VAL_2 = wmi_reqF_c_r + 2'b01 ;
  assign MUX_wmi_reqF_x_wire__wset_1__VAL_1 = { 20'b00111000000000000000, bl__h17580 } ;
  assign MUX_wmi_reqF_x_wire__wset_1__VAL_2 =
	     { 4'b0101, x__h18884, 1'b0, mesgReqAddr, fabWordsCurReq[11:0] } ;
  assign MUX_wmi_reqF_x_wire__wset_1__VAL_3 =
	     { 4'b0011, x__h16715, 1'b0, addr__h16647, 12'b000000000001 } ;
  always@(WILL_FIRE_RL_wmwt_requestPrecise or
	  MUX_wmi_reqF_x_wire__wset_1__VAL_1 or
	  WILL_FIRE_RL_wmrd_mesgBodyRequest or
	  MUX_wmi_reqF_x_wire__wset_1__VAL_2 or
	  WILL_FIRE_RL_wmwt_messagePushImprecise or
	  MUX_wmi_reqF_x_wire__wset_1__VAL_3)
  begin
    case (1'b1) // synopsys parallel_case
      WILL_FIRE_RL_wmwt_requestPrecise:
	  MUX_wmi_reqF_q_0__write_1__VAL_1 = MUX_wmi_reqF_x_wire__wset_1__VAL_1;
      WILL_FIRE_RL_wmrd_mesgBodyRequest:
	  MUX_wmi_reqF_q_0__write_1__VAL_1 = MUX_wmi_reqF_x_wire__wset_1__VAL_2;
      WILL_FIRE_RL_wmwt_messagePushImprecise:
	  MUX_wmi_reqF_q_0__write_1__VAL_1 = MUX_wmi_reqF_x_wire__wset_1__VAL_3;
      default: MUX_wmi_reqF_q_0__write_1__VAL_1 =
		   32'hAAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign MUX_wmi_reqF_q_0__write_1__VAL_2 =
	     (wmi_reqF_c_r == 2'b01) ?
	       MUX_wmi_reqF_q_0__write_1__VAL_1 :
	       wmi_reqF_q_1 ;
  assign MUX_wmi_reqF_q_1__write_1__VAL_2 =
	     (wmi_reqF_c_r == 2'b10) ?
	       MUX_wmi_reqF_q_0__write_1__VAL_1 :
	       32'b00001010101010101010101010101010 ;
  assign MUX_wsiM_reqFifo_x_wire__wset_1__VAL_1 =
	     { 3'b001,
	       unrollCnt == 16'b0000000000000001,
	       !smaCtrl[5],
	       x_burstLength__h18559,
	       wmi_respF__D_OUT[31:0],
	       x_byteEn__h18561,
	       thisMesg[23:16] } ;
  assign MUX_wsiM_reqFifo_q_0__write_1__VAL_2 =
	     MUX_wsiM_reqFifo_x_wire__wset_1__SEL_1 ?
	       MUX_wsiM_reqFifo_x_wire__wset_1__VAL_1 :
	       wsiS_reqFifo__D_OUT ;
  assign MUX_wsiM_reqFifo_q_0__write_1__VAL_1 =
	     (wsiM_reqFifo_c_r == 2'b01) ?
	       MUX_wsiM_reqFifo_q_0__write_1__VAL_2 :
	       wsiM_reqFifo_q_1 ;
  assign MUX_wsiM_reqFifo_q_1__write_1__VAL_1 =
	     (wsiM_reqFifo_c_r == 2'b10) ?
	       MUX_wsiM_reqFifo_q_0__write_1__VAL_2 :
	       61'h00000AAAAAAAAA00 ;
  assign MUX_wsiWordsRemain__write_1__VAL_2 = wsiWordsRemain - 12'b000000000001 ;
  assign MUX_wmi_reqF_q_0__write_1__SEL_1 =
	     WILL_FIRE_RL_wmi_reqF_incCtr && wmi_reqF_c_r == 2'b00 ;
  assign MUX_wmi_reqF_q_1__write_1__SEL_1 =
	     WILL_FIRE_RL_wmi_reqF_incCtr && wmi_reqF_c_r == 2'b01 ;
  assign MUX_wmi_mFlagF_q_0__write_1__SEL_2 =
	     WILL_FIRE_RL_wmi_mFlagF_incCtr && wmi_mFlagF_c_r == 2'b00 ;
  assign MUX_wmi_mFlagF_q_1__write_1__SEL_2 =
	     WILL_FIRE_RL_wmi_mFlagF_incCtr && wmi_mFlagF_c_r == 2'b01 ;
  assign MUX_wmi_dhF_q_0__write_1__SEL_2 =
	     WILL_FIRE_RL_wmi_dhF_incCtr && wmi_dhF_c_r == 2'b00 ;
  assign MUX_wmi_dhF_q_1__write_1__SEL_2 =
	     WILL_FIRE_RL_wmi_dhF_incCtr && wmi_dhF_c_r == 2'b01 ;

  // inlined wires
  ////
  //assign wci_wciReq__wget =
	//     { wciS0_MCmd,
	//       wciS0_MAddrSpace,
	//       wciS0_MByteEn,
	//       wciS0_MAddr,
	//       wciS0_MData } ;
  ////
  assign wci_wciReq__whas = 1'b1 ;
  ////
  //assign wci_reqF_r_enq__whas = CAN_FIRE_RL_wci_reqF_enq ;
  ////
  assign wci_reqF_r_clr__whas = 1'b0 ;
  assign wci_respF_dequeueing__whas = wci_respF_c_r != 2'b00 ;
  assign wci_wEdge__wget = wci_reqF__D_OUT[36:34] ;
  assign wci_sThreadBusy_pw__whas = 1'b0 ;
  assign wci_sFlagReg_1__wget = 1'b0 ;
  assign wci_sFlagReg_1__whas = 1'b0 ;
  assign wci_wci_cfwr_pw__whas =
	     wci_reqF__EMPTY_N && wci_reqF__D_OUT[56] &&
	     wci_reqF__D_OUT[59:57] == 3'b001 ;
  assign wci_wci_cfrd_pw__whas =
	     wci_reqF__EMPTY_N && wci_reqF__D_OUT[56] &&
	     wci_reqF__D_OUT[59:57] == 3'b010 ;
  assign wci_wci_ctrl_pw__whas =
	     wci_reqF__EMPTY_N && !wci_reqF__D_OUT[56] &&
	     wci_reqF__D_OUT[59:57] == 3'b010 ;
  assign wci_reqF_r_deq__whas =
	     WILL_FIRE_RL_wci_cfrd || WILL_FIRE_RL_wci_cfwr ||
	     WILL_FIRE_RL_wci_ctl_op_start ;
  assign wci_respF_enqueueing__whas =
	     WILL_FIRE_RL_wci_cfrd || WILL_FIRE_RL_wci_cfwr ||
	     WILL_FIRE_RL_wci_ctl_op_complete ;
  assign wci_respF_x_wire__whas =
	     WILL_FIRE_RL_wci_ctl_op_complete || WILL_FIRE_RL_wci_cfrd ||
	     WILL_FIRE_RL_wci_cfwr ;
  assign wci_ctlAckReg_1__wget = 1'b1 ;
  assign wci_wEdge__whas = WILL_FIRE_RL_wci_ctl_op_start ;
  assign wci_ctlAckReg_1__whas =
	     WILL_FIRE_RL_wci_ctrl_OrE || WILL_FIRE_RL_wci_ctrl_IsO ||
	     WILL_FIRE_RL_wci_ctrl_EiI ;
  assign wsiM_reqFifo_dequeueing__whas = CAN_FIRE_RL_wsiM_reqFifo_deq ;
  assign wsiM_sThreadBusy_pw__whas = wsiM1_SThreadBusy ;
  assign wsiM_operateD_1__wget = 1'b1 ;
  assign wsiM_operateD_1__whas = wci_cState == 3'b010 ;
  assign wsiM_peerIsReady_1__wget = 1'b1 ;
  assign wsiM_peerIsReady_1__whas = wsiM1_SReset_n ;
  assign wsiM_extStatusW__wget =
	     { wsiM_pMesgCount, wsiM_iMesgCount, wsiM_tBusyCount } ;
  ////
  //assign wsiS_wsiReq__wget =
	//     { wsiS1_MCmd,
	//       wsiS1_MReqLast,
	//       wsiS1_MBurstPrecise,
	//       wsiS1_MBurstLength,
	//       wsiS1_MData,
	//       wsiS1_MByteEn,
	//       wsiS1_MReqInfo } ;
  ////
  assign wsiS_wsiReq__whas = 1'b1 ;
  assign wsiS_reqFifo_r_enq__whas = CAN_FIRE_RL_wsiS_reqFifo_enq ;
  assign wsiS_reqFifo_r_clr__whas = 1'b0 ;
  assign wsiS_operateD_1__wget = 1'b1 ;
  assign wsiS_operateD_1__whas = wci_cState == 3'b010 ;
  assign wsiS_peerIsReady_1__wget = 1'b1 ;
  assign wsiS_extStatusW__wget =
	     { wsiS_pMesgCount, wsiS_iMesgCount, wsiS_tBusyCount } ;
  assign wsiS_peerIsReady_1__whas = wsiS1_MReset_n ;
  assign wsi_Es_mCmd_w__wget = wsiS1_MCmd ;
  assign wsi_Es_mReqLast_w__whas = wsiS1_MReqLast ;
  assign wsi_Es_mCmd_w__whas = 1'b1 ;
  assign wsi_Es_mBurstPrecise_w__whas = wsiS1_MBurstPrecise ;
  assign wsi_Es_mBurstLength_w__whas = 1'b1 ;
  assign wsi_Es_mBurstLength_w__wget = wsiS1_MBurstLength ;
  assign wsi_Es_mData_w__wget = wsiS1_MData ;
  assign wsi_Es_mData_w__whas = 1'b1 ;
  assign wsi_Es_mByteEn_w__wget = wsiS1_MByteEn ;
  assign wsi_Es_mByteEn_w__whas = 1'b1 ;
  assign wsi_Es_mReqInfo_w__whas = 1'b1 ;
  assign wsi_Es_mReqInfo_w__wget = wsiS1_MReqInfo ;
  assign wsi_Es_mDataInfo_w__whas = 1'b1 ;
  assign wci_respF_x_wire__wget = MUX_wci_respF_q_0__write_1__VAL_1 ;
  assign wsiM_reqFifo_enqueueing__whas =
	     (WILL_FIRE_RL_wsipass_doMessagePush ||
	      WILL_FIRE_RL_wmrd_mesgBodyResponse) &&
	     !smaCtrl[4] ;
  assign wsiM_reqFifo_x_wire__whas =
	     WILL_FIRE_RL_wmrd_mesgBodyResponse && !smaCtrl[4] ||
	     WILL_FIRE_RL_wsipass_doMessagePush && !smaCtrl[4] ;
  assign wsiS_reqFifo_r_deq__whas =
	     WILL_FIRE_RL_wsipass_doMessagePush ||
	     WILL_FIRE_RL_wmwt_messagePushPrecise ||
	     WILL_FIRE_RL_wmwt_messagePushImprecise ;
  assign wsiM_reqFifo_x_wire__wget = MUX_wsiM_reqFifo_q_0__write_1__VAL_2 ;
  assign wmi_reqF_enqueueing__whas =
	     WILL_FIRE_RL_wmwt_requestPrecise ||
	     WILL_FIRE_RL_wmwt_messagePushImprecise ||
	     WILL_FIRE_RL_wmrd_mesgBodyRequest ;
  assign wmi_reqF_x_wire__wget = MUX_wmi_reqF_q_0__write_1__VAL_1 ;
  assign wmi_reqF_x_wire__whas =
	     WILL_FIRE_RL_wmwt_requestPrecise ||
	     WILL_FIRE_RL_wmrd_mesgBodyRequest ||
	     WILL_FIRE_RL_wmwt_messagePushImprecise ;
  assign wmi_reqF_dequeueing__whas =
	     WILL_FIRE_RL_wmi_reqF_deq && wmi_reqF_c_r != 2'b00 ;
  assign wmi_mFlagF_x_wire__wget = value__h6065 ;
  assign wmi_mFlagF_enqueueing__whas =
	     WILL_FIRE_RL_wmwt_messagePushImprecise && x__h16715 ||
	     WILL_FIRE_RL_wmrd_mesgBodyRequest && x__h18884 ||
	     WILL_FIRE_RL_wmwt_requestPrecise ;
  assign wmi_mFlagF_x_wire__whas = wmi_mFlagF_enqueueing__whas ;
  assign wmi_mFlagF_dequeueing__whas =
	     WILL_FIRE_RL_wmi_reqF_deq && wmi_reqF_q_0[27] &&
	     wmi_mFlagF_c_r != 2'b00 ;
  assign wmi_dhF_enqueueing__whas =
	     WILL_FIRE_RL_wmwt_messagePushPrecise ||
	     WILL_FIRE_RL_wmwt_messagePushImprecise ;
  assign wmi_dhF_x_wire__wget = MUX_wmi_dhF_q_0__write_1__VAL_2 ;
  assign wmi_dhF_x_wire__whas = wmi_dhF_enqueueing__whas ;
  assign wmi_dhF_dequeueing__whas =
	     WILL_FIRE_RL_wmi_dhF_deq && wmi_dhF_c_r != 2'b00 ;
  ////
  //assign wmi_wmiResponse__wget = { wmiM_SResp, wmiM_SData } ;
  ////
  assign wmi_wmiResponse__whas = 1'b1 ;
  assign wmi_sThreadBusy_d_1__wget = 1'b1 ;
  assign wmi_sThreadBusy_d_1__whas = wmiM_SThreadBusy ;
  assign wmi_sDataThreadBusy_d_1__wget = 1'b1 ;
  assign wmi_operateD_1__wget = 1'b1 ;
  assign wmi_sDataThreadBusy_d_1__whas = wmiM_SDataThreadBusy ;
  assign wmi_operateD_1__whas = wci_cState == 3'b010 ;
  assign wmi_peerIsReady_1__whas = wmiM_SReset_n ;
  assign wmi_peerIsReady_1__wget = 1'b1 ;
  assign fabRespCredit_acc_v1__wget = b__h13937 ;
  assign fabRespCredit_acc_v2__wget = 4'b0001 ;
  assign fabRespCredit_acc_v1__whas = CAN_FIRE_RL_wmrd_mesgBodyRequest ;
  assign fabRespCredit_acc_v2__whas = CAN_FIRE_RL_wmrd_mesgBodyResponse ;
  assign mesgPreRequest_1__wget = 1'b1 ;
  assign mesgPreRequest_1__whas = WILL_FIRE_RL_wmrd_mesgBodyPreRequest ;
  assign wci_Es_mCmd_w__wget = wciS0_MCmd ;
  assign wci_Es_mAddrSpace_w__wget = wciS0_MAddrSpace ;
  assign wci_Es_mCmd_w__whas = 1'b1 ;
  assign wci_Es_mAddrSpace_w__whas = 1'b1 ;
  assign wci_Es_mAddr_w__wget = wciS0_MAddr ;
  assign wci_Es_mAddr_w__whas = 1'b1 ;
  assign wci_Es_mData_w__wget = wciS0_MData ;
  assign wci_Es_mData_w__whas = 1'b1 ;
  assign wci_Es_mByteEn_w__whas = 1'b1 ;
  assign wci_Es_mByteEn_w__wget = wciS0_MByteEn ;
  
  assign wmi_Em_sResp_w__whas = 1'b1 ;
  ////
  //assign wmi_Em_sData_w__wget = wmiM_SData ;
  //assign wmi_Em_sResp_w__wget = wmiM_SResp ;
  ////
  assign wmi_Em_sData_w__whas = 1'b1 ;

  // register abortCount
  assign abortCount__D_IN = abortCount + 32'b00000000000000000000000000000001 ;
  assign abortCount__EN = CAN_FIRE_RL_wmwt_doAbort ;

  // register doAbort
  assign doAbort__D_IN = 1'b0 ;
  assign doAbort__EN = CAN_FIRE_RL_wmwt_doAbort ;

  // register endOfMessage
  assign endOfMessage__D_IN = MUX_endOfMessage__write_1__SEL_1 ;
  assign endOfMessage__EN =
	     WILL_FIRE_RL_wmwt_messagePushImprecise && x__h16715 ||
	     WILL_FIRE_RL_wmwt_messageFinalize ;

  // register errCount
  assign errCount__D_IN = errCount + 32'b00000000000000000000000000000001 ;
  assign errCount__EN =
	     WILL_FIRE_RL_wmwt_messagePushImprecise &&
	     wsiS_reqFifo__D_OUT[43:12] != valExpect &&
	     (!x__h16715 || wsiS_reqFifo__D_OUT[11:8] != 4'b0000) ;

  // register fabRespCredit_value
  assign fabRespCredit_value__D_IN =
	     WILL_FIRE_RL_wci_ctrl_IsO ?
	       4'b0010 :
	       MUX_fabRespCredit_value__write_1__VAL_2 ;
  assign fabRespCredit_value__EN = 1'b1 ;

  // register fabWordsCurReq
  assign fabWordsCurReq__D_IN =
	     (fabWordsRemain <= b__h19084) ? fabWordsRemain : b__h19084 ;
  assign fabWordsCurReq__EN = WILL_FIRE_RL_wmrd_mesgBodyPreRequest ;

  // register fabWordsRemain
  assign fabWordsRemain__D_IN =
	     WILL_FIRE_RL_wmrd_mesgBegin ?
	       MUX_fabWordsRemain__write_1__VAL_1 :
	       MUX_fabWordsRemain__write_1__VAL_2 ;
  assign fabWordsRemain__EN =
	     WILL_FIRE_RL_wmrd_mesgBegin ||
	     WILL_FIRE_RL_wmrd_mesgBodyRequest ;

  // register firstMsgReq
  assign firstMsgReq__EN = 1'b0 ;
  assign firstMsgReq__D_IN = 1'b0 ;

  // register impreciseBurst
  always@(WILL_FIRE_RL_wmwt_doAbort or
	  MUX_impreciseBurst__write_1__SEL_2 or
	  WILL_FIRE_RL_wmwt_messageFinalize)
  case (1'b1)
    WILL_FIRE_RL_wmwt_doAbort: impreciseBurst__D_IN = 1'b0;
    MUX_impreciseBurst__write_1__SEL_2: impreciseBurst__D_IN = 1'b1;
    WILL_FIRE_RL_wmwt_messageFinalize: impreciseBurst__D_IN = 1'b0;
    default: impreciseBurst__D_IN = 1'b0 /* unspecified value */ ;
  endcase
  assign impreciseBurst__EN =
	     WILL_FIRE_RL_wmwt_mesgBegin && !wsiS_reqFifo__D_OUT[56] ||
	     WILL_FIRE_RL_wmwt_messageFinalize ||
	     WILL_FIRE_RL_wmwt_doAbort ;

  // register lastMesg
  assign lastMesg__D_IN =
	     (MUX_endOfMessage__write_1__SEL_1 ||
	      MUX_lastMesg__write_1__SEL_2) ?
	       thisMesg :
	       32'hFEFEFFFE ;
  assign lastMesg__EN =
	     WILL_FIRE_RL_wmwt_messagePushImprecise && x__h16715 ||
	     WILL_FIRE_RL_wmwt_requestPrecise ||
	     WILL_FIRE_RL_wmrd_mesgBegin ||
	     WILL_FIRE_RL_wci_ctrl_IsO ;

  // register mesgCount
  always@(MUX_mesgCount__write_1__SEL_1 or
	  MUX_mesgCount__write_1__VAL_1 or
	  WILL_FIRE_RL_wmwt_messageFinalize or WILL_FIRE_RL_wci_ctrl_IsO)
  begin
    case (1'b1) // synopsys parallel_case
      MUX_mesgCount__write_1__SEL_1:
	  mesgCount__D_IN = MUX_mesgCount__write_1__VAL_1;
      WILL_FIRE_RL_wmwt_messageFinalize:
	  mesgCount__D_IN = MUX_mesgCount__write_1__VAL_1;
      WILL_FIRE_RL_wci_ctrl_IsO: mesgCount__D_IN = 32'b00000000000000000000000000000000;
      default: mesgCount__D_IN = 32'hAAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign mesgCount__EN =
	     WILL_FIRE_RL_wmrd_mesgBodyResponse && unrollCnt == 16'b0000000000000001 ||
	     WILL_FIRE_RL_wmwt_messageFinalize ||
	     WILL_FIRE_RL_wci_ctrl_IsO ;

  // register mesgLength
  always@(WILL_FIRE_RL_wmwt_doAbort or
	  MUX_mesgLength__write_1__SEL_2 or
	  MUX_mesgLength__write_1__VAL_2 or
	  WILL_FIRE_RL_wmwt_messageFinalize or
	  MUX_endOfMessage__write_1__SEL_1 or MUX_mesgLength__write_1__VAL_4)
  case (1'b1)
    WILL_FIRE_RL_wmwt_doAbort: mesgLength__D_IN = 15'b010101010101010;
    MUX_mesgLength__write_1__SEL_2:
	mesgLength__D_IN = MUX_mesgLength__write_1__VAL_2;
    WILL_FIRE_RL_wmwt_messageFinalize: mesgLength__D_IN = 15'b010101010101010;
    MUX_endOfMessage__write_1__SEL_1:
	mesgLength__D_IN = MUX_mesgLength__write_1__VAL_4;
    default: mesgLength__D_IN = 15'b010101010101010 /* unspecified value */ ;
  endcase
  assign mesgLength__EN =
	     WILL_FIRE_RL_wmwt_messagePushImprecise && x__h16715 ||
	     WILL_FIRE_RL_wmwt_mesgBegin && wsiS_reqFifo__D_OUT[56] ||
	     WILL_FIRE_RL_wmwt_messageFinalize ||
	     WILL_FIRE_RL_wmwt_doAbort ;

  // register mesgLengthSoFar
  assign mesgLengthSoFar__D_IN =
	     MUX_impreciseBurst__write_1__SEL_2 ? 14'b00000000000000 : mlp1__h16630 ;
  assign mesgLengthSoFar__EN =
	     WILL_FIRE_RL_wmwt_mesgBegin && !wsiS_reqFifo__D_OUT[56] ||
	     WILL_FIRE_RL_wmwt_messagePushImprecise ;

  // register mesgPreRequest
  assign mesgPreRequest__D_IN = WILL_FIRE_RL_wmrd_mesgBodyPreRequest ;
  assign mesgPreRequest__EN = 1'b1 ;

  // register mesgReqAddr
  assign mesgReqAddr__D_IN =
	     WILL_FIRE_RL_wmrd_mesgBegin ?
	       14'b00000000000000 :
	       MUX_mesgReqAddr__write_1__VAL_2 ;
  assign mesgReqAddr__EN =
	     WILL_FIRE_RL_wmrd_mesgBodyRequest ||
	     WILL_FIRE_RL_wmrd_mesgBegin ;

  // register mesgReqOK
  assign mesgReqOK__D_IN =
	     WILL_FIRE_RL_wmrd_mesgBodyResponse ||
	     WILL_FIRE_RL_wmrd_mesgBegin ;
  assign mesgReqOK__EN =
	     WILL_FIRE_RL_wmrd_mesgBodyPreRequest ||
	     WILL_FIRE_RL_wmrd_mesgBegin ||
	     WILL_FIRE_RL_wmrd_mesgBodyResponse ;

  // register mesgReqValid
  assign mesgReqValid__D_IN = !WILL_FIRE_RL_wmwt_messageFinalize ;
  assign mesgReqValid__EN =
	     WILL_FIRE_RL_wmwt_messageFinalize ||
	     WILL_FIRE_RL_wmwt_requestPrecise ;

  // register opcode
  always@(WILL_FIRE_RL_wmwt_doAbort or
	  WILL_FIRE_RL_wmwt_mesgBegin or
	  MUX_opcode__write_1__VAL_2 or WILL_FIRE_RL_wmwt_messageFinalize)
  case (1'b1)
    WILL_FIRE_RL_wmwt_doAbort: opcode__D_IN = 9'b010101010;
    WILL_FIRE_RL_wmwt_mesgBegin: opcode__D_IN = MUX_opcode__write_1__VAL_2;
    WILL_FIRE_RL_wmwt_messageFinalize: opcode__D_IN = 9'b010101010;
    default: opcode__D_IN = 9'b010101010 /* unspecified value */ ;
  endcase
  assign opcode__EN =
	     WILL_FIRE_RL_wmwt_mesgBegin ||
	     WILL_FIRE_RL_wmwt_messageFinalize ||
	     WILL_FIRE_RL_wmwt_doAbort ;

  // register preciseBurst
  always@(WILL_FIRE_RL_wmwt_doAbort or
	  MUX_mesgLength__write_1__SEL_2 or WILL_FIRE_RL_wmwt_messageFinalize)
  case (1'b1)
    WILL_FIRE_RL_wmwt_doAbort: preciseBurst__D_IN = 1'b0;
    MUX_mesgLength__write_1__SEL_2: preciseBurst__D_IN = 1'b1;
    WILL_FIRE_RL_wmwt_messageFinalize: preciseBurst__D_IN = 1'b0;
    default: preciseBurst__D_IN = 1'b0 /* unspecified value */ ;
  endcase
  assign preciseBurst__EN =
	     WILL_FIRE_RL_wmwt_mesgBegin && wsiS_reqFifo__D_OUT[56] ||
	     WILL_FIRE_RL_wmwt_messageFinalize ||
	     WILL_FIRE_RL_wmwt_doAbort ;

  // register readyToPush
  always@(WILL_FIRE_RL_wmwt_doAbort or
	  MUX_impreciseBurst__write_1__SEL_2 or
	  MUX_endOfMessage__write_1__SEL_1)
  case (1'b1)
    WILL_FIRE_RL_wmwt_doAbort: readyToPush__D_IN = 1'b0;
    MUX_impreciseBurst__write_1__SEL_2: readyToPush__D_IN = 1'b1;
    MUX_endOfMessage__write_1__SEL_1: readyToPush__D_IN = 1'b0;
    default: readyToPush__D_IN = 1'b0 /* unspecified value */ ;
  endcase
  assign readyToPush__EN =
	     WILL_FIRE_RL_wmwt_messagePushImprecise && x__h16715 ||
	     WILL_FIRE_RL_wmwt_mesgBegin && !wsiS_reqFifo__D_OUT[56] ||
	     WILL_FIRE_RL_wmwt_doAbort ;

  // register readyToRequest
  assign readyToRequest__D_IN = MUX_mesgLength__write_1__SEL_2 ;
  assign readyToRequest__EN =
	     WILL_FIRE_RL_wmwt_mesgBegin && wsiS_reqFifo__D_OUT[56] ||
	     WILL_FIRE_RL_wmwt_requestPrecise ;

  // register smaCtrl
  assign smaCtrl__D_IN = wci_reqF__D_OUT[31:0] ;
  assign smaCtrl__EN = WILL_FIRE_RL_wci_cfwr && wci_reqF__D_OUT[39:32] == 8'h0 ;

  // register thisMesg
  always@(MUX_endOfMessage__write_1__SEL_1 or
	  MUX_thisMesg__write_1__VAL_1 or
	  WILL_FIRE_RL_wmrd_mesgBegin or
	  MUX_thisMesg__write_1__VAL_2 or
	  WILL_FIRE_RL_wmwt_requestPrecise or WILL_FIRE_RL_wci_ctrl_IsO)
  begin
    case (1'b1) // synopsys parallel_case
      MUX_endOfMessage__write_1__SEL_1:
	  thisMesg__D_IN = MUX_thisMesg__write_1__VAL_1;
      WILL_FIRE_RL_wmrd_mesgBegin:
	  thisMesg__D_IN = MUX_thisMesg__write_1__VAL_2;
      WILL_FIRE_RL_wmwt_requestPrecise:
	  thisMesg__D_IN = MUX_thisMesg__write_1__VAL_1;
      WILL_FIRE_RL_wci_ctrl_IsO: thisMesg__D_IN = 32'hFEFEFFFE;
      default: thisMesg__D_IN = 32'hAAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign thisMesg__EN =
	     WILL_FIRE_RL_wmwt_messagePushImprecise && x__h16715 ||
	     WILL_FIRE_RL_wmrd_mesgBegin ||
	     WILL_FIRE_RL_wmwt_requestPrecise ||
	     WILL_FIRE_RL_wci_ctrl_IsO ;

  // register unrollCnt
  assign unrollCnt__D_IN =
	     WILL_FIRE_RL_wmrd_mesgBegin ?
	       MUX_unrollCnt__write_1__VAL_1 :
	       MUX_unrollCnt__write_1__VAL_2 ;
  assign unrollCnt__EN =
	     WILL_FIRE_RL_wmrd_mesgBegin ||
	     WILL_FIRE_RL_wmrd_mesgBodyResponse ;

  // register valExpect
  assign valExpect__D_IN = valExpect + 32'b00000000000000000000000000000001 ;
  assign valExpect__EN =
	     WILL_FIRE_RL_wmwt_messagePushImprecise &&
	     (!x__h16715 || wsiS_reqFifo__D_OUT[11:8] != 4'b0000) ;

  // register wci_cEdge
  assign wci_cEdge__D_IN = wci_reqF__D_OUT[36:34] ;
  assign wci_cEdge__EN = WILL_FIRE_RL_wci_ctl_op_start ;

  // register wci_cState
  assign wci_cState__D_IN = wci_nState ;
  assign wci_cState__EN =
	     WILL_FIRE_RL_wci_ctl_op_complete && !wci_illegalEdge ;

  // register wci_ctlAckReg
  assign wci_ctlAckReg__D_IN = wci_ctlAckReg_1__whas ;
  assign wci_ctlAckReg__EN = 1'b1 ;

  // register wci_ctlOpActive
  assign wci_ctlOpActive__D_IN = !WILL_FIRE_RL_wci_ctl_op_complete ;
  assign wci_ctlOpActive__EN =
	     WILL_FIRE_RL_wci_ctl_op_complete ||
	     WILL_FIRE_RL_wci_ctl_op_start ;

  // register wci_illegalEdge
  assign wci_illegalEdge__D_IN =
	     !MUX_wci_illegalEdge__write_1__SEL_1 &&
	     MUX_wci_illegalEdge__write_1__VAL_2 ;
  assign wci_illegalEdge__EN =
	     WILL_FIRE_RL_wci_ctl_op_complete && wci_illegalEdge ||
	     MUX_wci_illegalEdge__write_1__SEL_2 ;

  // register wci_nState
  always@(wci_reqF__D_OUT)
  begin
    case (wci_reqF__D_OUT[36:34])
      3'b000: wci_nState__D_IN = 3'b001;
      3'b001: wci_nState__D_IN = 3'b010;
      3'b010: wci_nState__D_IN = 3'b011;
      default: wci_nState__D_IN = 3'b000;
    endcase
  end
  assign wci_nState__EN =
	     WILL_FIRE_RL_wci_ctl_op_start &&
	     (wci_reqF__D_OUT[36:34] == 3'b000 && wci_cState == 3'b000 ||
	      wci_reqF__D_OUT[36:34] == 3'b001 &&
	      (wci_cState == 3'b001 || wci_cState == 3'b011) ||
	      wci_reqF__D_OUT[36:34] == 3'b010 && wci_cState == 3'b010 ||
	      wci_reqF__D_OUT[36:34] == 3'b011 &&
	      (wci_cState == 3'b011 || wci_cState == 3'b010 ||
	       wci_cState == 3'b001)) ;

  // register wci_reqF_countReg
  assign wci_reqF_countReg__D_IN =
	     (wciS0_MCmd[2:0] != 3'b000) ?
	       wci_reqF_countReg + 2'b01 :
	       wci_reqF_countReg - 2'b01 ;
  assign wci_reqF_countReg__EN = CAN_FIRE_RL_wci_reqF__updateLevelCounter ;

  // register wci_respF_c_r
  assign wci_respF_c_r__D_IN =
	     WILL_FIRE_RL_wci_respF_decCtr ?
	       MUX_wci_respF_c_r__write_1__VAL_1 :
	       MUX_wci_respF_c_r__write_1__VAL_2 ;
  assign wci_respF_c_r__EN =
	     WILL_FIRE_RL_wci_respF_decCtr || WILL_FIRE_RL_wci_respF_incCtr ;

  // register wci_respF_q_0
  assign wci_respF_q_0__EN =
	     WILL_FIRE_RL_wci_respF_incCtr && wci_respF_c_r == 2'b00 ||
	     WILL_FIRE_RL_wci_respF_both ||
	     WILL_FIRE_RL_wci_respF_decCtr ;
  always@(MUX_wci_respF_q_0__write_1__SEL_1 or
	  MUX_wci_respF_q_0__write_1__VAL_1 or
	  WILL_FIRE_RL_wci_respF_both or
	  MUX_wci_respF_q_0__write_1__VAL_2 or
	  WILL_FIRE_RL_wci_respF_decCtr or wci_respF_q_1)
  begin
    case (1'b1) // synopsys parallel_case
      MUX_wci_respF_q_0__write_1__SEL_1:
	  wci_respF_q_0__D_IN = MUX_wci_respF_q_0__write_1__VAL_1;
      WILL_FIRE_RL_wci_respF_both:
	  wci_respF_q_0__D_IN = MUX_wci_respF_q_0__write_1__VAL_2;
      WILL_FIRE_RL_wci_respF_decCtr: wci_respF_q_0__D_IN = wci_respF_q_1;
      default: wci_respF_q_0__D_IN = 34'h2AAAAAAAA /* unspecified value */ ;
    endcase
  end

  // register wci_respF_q_1
  assign wci_respF_q_1__EN =
	     WILL_FIRE_RL_wci_respF_incCtr && wci_respF_c_r == 2'b01 ||
	     WILL_FIRE_RL_wci_respF_both ||
	     WILL_FIRE_RL_wci_respF_decCtr ;
  always@(MUX_wci_respF_q_1__write_1__SEL_1 or
	  MUX_wci_respF_q_0__write_1__VAL_1 or
	  WILL_FIRE_RL_wci_respF_both or
	  MUX_wci_respF_q_1__write_1__VAL_2 or WILL_FIRE_RL_wci_respF_decCtr)
  begin
    case (1'b1) // synopsys parallel_case
      MUX_wci_respF_q_1__write_1__SEL_1:
	  wci_respF_q_1__D_IN = MUX_wci_respF_q_0__write_1__VAL_1;
      WILL_FIRE_RL_wci_respF_both:
	  wci_respF_q_1__D_IN = MUX_wci_respF_q_1__write_1__VAL_2;
      WILL_FIRE_RL_wci_respF_decCtr: wci_respF_q_1__D_IN = 34'h0AAAAAAAA;
      default: wci_respF_q_1__D_IN = 34'h2AAAAAAAA /* unspecified value */ ;
    endcase
  end

  // register wci_sFlagReg
  assign wci_sFlagReg__D_IN = 1'b0 ;
  assign wci_sFlagReg__EN = 1'b1 ;

  // register wci_sThreadBusy_d
  assign wci_sThreadBusy_d__D_IN = 1'b0 ;
  assign wci_sThreadBusy_d__EN = 1'b1 ;

  // register wmi_busyWithMessage
  assign wmi_busyWithMessage__D_IN = 1'b0 ;
  assign wmi_busyWithMessage__EN = 1'b0 ;

  // register wmi_dhF_c_r
  assign wmi_dhF_c_r__D_IN =
	     WILL_FIRE_RL_wmi_dhF_decCtr ?
	       MUX_wmi_dhF_c_r__write_1__VAL_1 :
	       MUX_wmi_dhF_c_r__write_1__VAL_2 ;
  assign wmi_dhF_c_r__EN =
	     WILL_FIRE_RL_wmi_dhF_decCtr || WILL_FIRE_RL_wmi_dhF_incCtr ;

  // register wmi_dhF_q_0
  always@(WILL_FIRE_RL_wmi_dhF_both or
	  MUX_wmi_dhF_q_0__write_1__VAL_1 or
	  MUX_wmi_dhF_q_0__write_1__SEL_2 or
	  MUX_wmi_dhF_q_0__write_1__VAL_2 or
	  WILL_FIRE_RL_wmi_dhF_decCtr or wmi_dhF_q_1)
  begin
    case (1'b1) // synopsys parallel_case
      WILL_FIRE_RL_wmi_dhF_both:
	  wmi_dhF_q_0__D_IN = MUX_wmi_dhF_q_0__write_1__VAL_1;
      MUX_wmi_dhF_q_0__write_1__SEL_2:
	  wmi_dhF_q_0__D_IN = MUX_wmi_dhF_q_0__write_1__VAL_2;
      WILL_FIRE_RL_wmi_dhF_decCtr: wmi_dhF_q_0__D_IN = wmi_dhF_q_1;
      default: wmi_dhF_q_0__D_IN = 38'h2AAAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign wmi_dhF_q_0__EN =
	     WILL_FIRE_RL_wmi_dhF_both ||
	     WILL_FIRE_RL_wmi_dhF_incCtr && wmi_dhF_c_r == 2'b00 ||
	     WILL_FIRE_RL_wmi_dhF_decCtr ;

  // register wmi_dhF_q_1
  always@(WILL_FIRE_RL_wmi_dhF_both or
	  MUX_wmi_dhF_q_1__write_1__VAL_1 or
	  MUX_wmi_dhF_q_1__write_1__SEL_2 or
	  MUX_wmi_dhF_q_0__write_1__VAL_2 or WILL_FIRE_RL_wmi_dhF_decCtr)
  begin
    case (1'b1) // synopsys parallel_case
      WILL_FIRE_RL_wmi_dhF_both:
	  wmi_dhF_q_1__D_IN = MUX_wmi_dhF_q_1__write_1__VAL_1;
      MUX_wmi_dhF_q_1__write_1__SEL_2:
	  wmi_dhF_q_1__D_IN = MUX_wmi_dhF_q_0__write_1__VAL_2;
      WILL_FIRE_RL_wmi_dhF_decCtr: wmi_dhF_q_1__D_IN = 38'h0AAAAAAAAA;
      default: wmi_dhF_q_1__D_IN = 38'h2AAAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign wmi_dhF_q_1__EN =
	     WILL_FIRE_RL_wmi_dhF_both ||
	     WILL_FIRE_RL_wmi_dhF_incCtr && wmi_dhF_c_r == 2'b01 ||
	     WILL_FIRE_RL_wmi_dhF_decCtr ;

  // register wmi_mFlagF_c_r
  assign wmi_mFlagF_c_r__D_IN =
	     WILL_FIRE_RL_wmi_mFlagF_decCtr ?
	       MUX_wmi_mFlagF_c_r__write_1__VAL_1 :
	       MUX_wmi_mFlagF_c_r__write_1__VAL_2 ;
  assign wmi_mFlagF_c_r__EN =
	     WILL_FIRE_RL_wmi_mFlagF_decCtr ||
	     WILL_FIRE_RL_wmi_mFlagF_incCtr ;

  // register wmi_mFlagF_q_0
  always@(WILL_FIRE_RL_wmi_mFlagF_both or
	  MUX_wmi_mFlagF_q_0__write_1__VAL_1 or
	  MUX_wmi_mFlagF_q_0__write_1__SEL_2 or
	  value__h6065 or WILL_FIRE_RL_wmi_mFlagF_decCtr or wmi_mFlagF_q_1)
  begin
    case (1'b1) // synopsys parallel_case
      WILL_FIRE_RL_wmi_mFlagF_both:
	  wmi_mFlagF_q_0__D_IN = MUX_wmi_mFlagF_q_0__write_1__VAL_1;
      MUX_wmi_mFlagF_q_0__write_1__SEL_2: wmi_mFlagF_q_0__D_IN = value__h6065;
      WILL_FIRE_RL_wmi_mFlagF_decCtr: wmi_mFlagF_q_0__D_IN = wmi_mFlagF_q_1;
      default: wmi_mFlagF_q_0__D_IN = 32'hAAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign wmi_mFlagF_q_0__EN =
	     WILL_FIRE_RL_wmi_mFlagF_both ||
	     WILL_FIRE_RL_wmi_mFlagF_incCtr && wmi_mFlagF_c_r == 2'b00 ||
	     WILL_FIRE_RL_wmi_mFlagF_decCtr ;

  // register wmi_mFlagF_q_1
  always@(WILL_FIRE_RL_wmi_mFlagF_both or
	  MUX_wmi_mFlagF_q_1__write_1__VAL_1 or
	  MUX_wmi_mFlagF_q_1__write_1__SEL_2 or
	  value__h6065 or WILL_FIRE_RL_wmi_mFlagF_decCtr)
  begin
    case (1'b1) // synopsys parallel_case
      WILL_FIRE_RL_wmi_mFlagF_both:
	  wmi_mFlagF_q_1__D_IN = MUX_wmi_mFlagF_q_1__write_1__VAL_1;
      MUX_wmi_mFlagF_q_1__write_1__SEL_2: wmi_mFlagF_q_1__D_IN = value__h6065;
      WILL_FIRE_RL_wmi_mFlagF_decCtr: wmi_mFlagF_q_1__D_IN = 32'b00000000000000000000000000000000;
      default: wmi_mFlagF_q_1__D_IN = 32'hAAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign wmi_mFlagF_q_1__EN =
	     WILL_FIRE_RL_wmi_mFlagF_both ||
	     WILL_FIRE_RL_wmi_mFlagF_incCtr && wmi_mFlagF_c_r == 2'b01 ||
	     WILL_FIRE_RL_wmi_mFlagF_decCtr ;

  // register wmi_operateD
  assign wmi_operateD__D_IN = wci_cState == 3'b010 ;
  assign wmi_operateD__EN = 1'b1 ;

  // register wmi_peerIsReady
  assign wmi_peerIsReady__D_IN = wmiM_SReset_n ;
  assign wmi_peerIsReady__EN = 1'b1 ;

  // register wmi_reqF_c_r
  assign wmi_reqF_c_r__D_IN =
	     WILL_FIRE_RL_wmi_reqF_decCtr ?
	       MUX_wmi_reqF_c_r__write_1__VAL_1 :
	       MUX_wmi_reqF_c_r__write_1__VAL_2 ;
  assign wmi_reqF_c_r__EN =
	     WILL_FIRE_RL_wmi_reqF_decCtr || WILL_FIRE_RL_wmi_reqF_incCtr ;

  // register wmi_reqF_q_0
  always@(MUX_wmi_reqF_q_0__write_1__SEL_1 or
	  MUX_wmi_reqF_q_0__write_1__VAL_1 or
	  WILL_FIRE_RL_wmi_reqF_both or
	  MUX_wmi_reqF_q_0__write_1__VAL_2 or
	  WILL_FIRE_RL_wmi_reqF_decCtr or wmi_reqF_q_1)
  begin
    case (1'b1) // synopsys parallel_case
      MUX_wmi_reqF_q_0__write_1__SEL_1:
	  wmi_reqF_q_0__D_IN = MUX_wmi_reqF_q_0__write_1__VAL_1;
      WILL_FIRE_RL_wmi_reqF_both:
	  wmi_reqF_q_0__D_IN = MUX_wmi_reqF_q_0__write_1__VAL_2;
      WILL_FIRE_RL_wmi_reqF_decCtr: wmi_reqF_q_0__D_IN = wmi_reqF_q_1;
      default: wmi_reqF_q_0__D_IN = 32'hAAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign wmi_reqF_q_0__EN =
	     WILL_FIRE_RL_wmi_reqF_incCtr && wmi_reqF_c_r == 2'b00 ||
	     WILL_FIRE_RL_wmi_reqF_both ||
	     WILL_FIRE_RL_wmi_reqF_decCtr ;

  // register wmi_reqF_q_1
  always@(MUX_wmi_reqF_q_1__write_1__SEL_1 or
	  MUX_wmi_reqF_q_0__write_1__VAL_1 or
	  WILL_FIRE_RL_wmi_reqF_both or
	  MUX_wmi_reqF_q_1__write_1__VAL_2 or WILL_FIRE_RL_wmi_reqF_decCtr)
  begin
    case (1'b1) // synopsys parallel_case
      MUX_wmi_reqF_q_1__write_1__SEL_1:
	  wmi_reqF_q_1__D_IN = MUX_wmi_reqF_q_0__write_1__VAL_1;
      WILL_FIRE_RL_wmi_reqF_both:
	  wmi_reqF_q_1__D_IN = MUX_wmi_reqF_q_1__write_1__VAL_2;
      WILL_FIRE_RL_wmi_reqF_decCtr: wmi_reqF_q_1__D_IN = 32'b00001010101010101010101010101010;
      default: wmi_reqF_q_1__D_IN = 32'hAAAAAAAA /* unspecified value */ ;
    endcase
  end
  assign wmi_reqF_q_1__EN =
	     WILL_FIRE_RL_wmi_reqF_incCtr && wmi_reqF_c_r == 2'b01 ||
	     WILL_FIRE_RL_wmi_reqF_both ||
	     WILL_FIRE_RL_wmi_reqF_decCtr ;

  // register wmi_sDataThreadBusy_d
  assign wmi_sDataThreadBusy_d__D_IN = wmiM_SDataThreadBusy ;
  assign wmi_sDataThreadBusy_d__EN = 1'b1 ;

  // register wmi_sFlagReg
  assign wmi_sFlagReg__D_IN = wmiM_SFlag ;
  assign wmi_sFlagReg__EN = 1'b1 ;

  // register wmi_sThreadBusy_d
  assign wmi_sThreadBusy_d__D_IN = wmiM_SThreadBusy ;
  assign wmi_sThreadBusy_d__EN = 1'b1 ;

  // register wsiM_burstKind
  assign wsiM_burstKind__D_IN =
	     (wsiM_burstKind == 2'b00) ?
	       (wsiM_reqFifo_q_0[56] ? 2'b01 : 2'b10) :
	       2'b00 ;
  assign wsiM_burstKind__EN =
	     WILL_FIRE_RL_wsiM_reqFifo_deq &&
	     wsiM_reqFifo_q_0[60:58] == 3'b001 &&
	     (wsiM_burstKind == 2'b00 ||
	      wsiM_burstKind == 2'b01 && wsiM_reqFifo_q_0[57] ||
	      wsiM_burstKind == 2'b10 && wsiM_reqFifo_q_0[55:44] == 12'b000000000001) ;

  // register wsiM_errorSticky
  assign wsiM_errorSticky__D_IN = 1'b0 ;
  assign wsiM_errorSticky__EN = 1'b0 ;

  // register wsiM_iMesgCount
  assign wsiM_iMesgCount__D_IN = wsiM_iMesgCount + 32'b00000000000000000000000000000001 ;
  assign wsiM_iMesgCount__EN =
	     WILL_FIRE_RL_wsiM_reqFifo_deq &&
	     wsiM_reqFifo_q_0[60:58] == 3'b001 &&
	     wsiM_burstKind == 2'b10 &&
	     wsiM_reqFifo_q_0[55:44] == 12'b000000000001 ;

  // register wsiM_operateD
  assign wsiM_operateD__D_IN = wci_cState == 3'b010 ;
  assign wsiM_operateD__EN = 1'b1 ;

  // register wsiM_pMesgCount
  assign wsiM_pMesgCount__D_IN = wsiM_pMesgCount + 32'b00000000000000000000000000000001 ;
  assign wsiM_pMesgCount__EN =
	     WILL_FIRE_RL_wsiM_reqFifo_deq &&
	     wsiM_reqFifo_q_0[60:58] == 3'b001 &&
	     wsiM_burstKind == 2'b01 &&
	     wsiM_reqFifo_q_0[57] ;

  // register wsiM_peerIsReady
  assign wsiM_peerIsReady__D_IN = wsiM1_SReset_n ;
  assign wsiM_peerIsReady__EN = 1'b1 ;

  // register wsiM_reqFifo_c_r
  assign wsiM_reqFifo_c_r__D_IN =
	     WILL_FIRE_RL_wsiM_reqFifo_decCtr ?
	       MUX_wsiM_reqFifo_c_r__write_1__VAL_1 :
	       MUX_wsiM_reqFifo_c_r__write_1__VAL_2 ;
  assign wsiM_reqFifo_c_r__EN =
	     WILL_FIRE_RL_wsiM_reqFifo_decCtr ||
	     WILL_FIRE_RL_wsiM_reqFifo_incCtr ;

  // register wsiM_reqFifo_q_0
  assign wsiM_reqFifo_q_0__EN =
	     WILL_FIRE_RL_wsiM_reqFifo_both ||
	     WILL_FIRE_RL_wsiM_reqFifo_incCtr && wsiM_reqFifo_c_r == 2'b00 ||
	     WILL_FIRE_RL_wsiM_reqFifo_decCtr ;
  always@(WILL_FIRE_RL_wsiM_reqFifo_both or
	  MUX_wsiM_reqFifo_q_0__write_1__VAL_1 or
	  MUX_wsiM_reqFifo_q_0__write_1__SEL_2 or
	  MUX_wsiM_reqFifo_q_0__write_1__VAL_2 or
	  WILL_FIRE_RL_wsiM_reqFifo_decCtr or wsiM_reqFifo_q_1)
  begin
    case (1'b1) // synopsys parallel_case
      WILL_FIRE_RL_wsiM_reqFifo_both:
	  wsiM_reqFifo_q_0__D_IN = MUX_wsiM_reqFifo_q_0__write_1__VAL_1;
      MUX_wsiM_reqFifo_q_0__write_1__SEL_2:
	  wsiM_reqFifo_q_0__D_IN = MUX_wsiM_reqFifo_q_0__write_1__VAL_2;
      WILL_FIRE_RL_wsiM_reqFifo_decCtr:
	  wsiM_reqFifo_q_0__D_IN = wsiM_reqFifo_q_1;
      default: wsiM_reqFifo_q_0__D_IN =
		   61'h0AAAAAAAAAAAAAAA /* unspecified value */ ;
    endcase
  end

  // register wsiM_reqFifo_q_1
  assign wsiM_reqFifo_q_1__EN =
	     WILL_FIRE_RL_wsiM_reqFifo_both ||
	     WILL_FIRE_RL_wsiM_reqFifo_incCtr && wsiM_reqFifo_c_r == 2'b01 ||
	     WILL_FIRE_RL_wsiM_reqFifo_decCtr ;
  always@(WILL_FIRE_RL_wsiM_reqFifo_both or
	  MUX_wsiM_reqFifo_q_1__write_1__VAL_1 or
	  MUX_wsiM_reqFifo_q_1__write_1__SEL_2 or
	  MUX_wsiM_reqFifo_q_0__write_1__VAL_2 or
	  WILL_FIRE_RL_wsiM_reqFifo_decCtr)
  begin
    case (1'b1) // synopsys parallel_case
      WILL_FIRE_RL_wsiM_reqFifo_both:
	  wsiM_reqFifo_q_1__D_IN = MUX_wsiM_reqFifo_q_1__write_1__VAL_1;
      MUX_wsiM_reqFifo_q_1__write_1__SEL_2:
	  wsiM_reqFifo_q_1__D_IN = MUX_wsiM_reqFifo_q_0__write_1__VAL_2;
      WILL_FIRE_RL_wsiM_reqFifo_decCtr:
	  wsiM_reqFifo_q_1__D_IN = 61'h00000AAAAAAAAA00;
      default: wsiM_reqFifo_q_1__D_IN =
		   61'h0AAAAAAAAAAAAAAA /* unspecified value */ ;
    endcase
  end

  // register wsiM_sThreadBusy_d
  assign wsiM_sThreadBusy_d__D_IN = wsiM1_SThreadBusy ;
  assign wsiM_sThreadBusy_d__EN = 1'b1 ;

  // register wsiM_statusR
  assign wsiM_statusR__D_IN =
	     { wsiM_isReset__VAL,
	       !wsiM_peerIsReady,
	       !wsiM_operateD,
	       wsiM_errorSticky,
	       wsiM_burstKind != 2'b00,
	       wsiM_sThreadBusy_d,
	       1'b0,
	       wsiM_trafficSticky } ;
  assign wsiM_statusR__EN = 1'b1 ;

  // register wsiM_tBusyCount
  assign wsiM_tBusyCount__D_IN = wsiM_tBusyCount + 32'b00000000000000000000000000000001 ;
  assign wsiM_tBusyCount__EN = CAN_FIRE_RL_wsiM_inc_tBusyCount ;

  // register wsiM_trafficSticky
  assign wsiM_trafficSticky__D_IN = 1'b1 ;
  assign wsiM_trafficSticky__EN =
	     WILL_FIRE_RL_wsiM_reqFifo_deq &&
	     wsiM_reqFifo_q_0[60:58] == 3'b001 ;

  // register wsiS_burstKind
  assign wsiS_burstKind__D_IN =
	     (wsiS_burstKind == 2'b00) ?
	       (wsiS1_MBurstPrecise ? 2'b01 : 2'b10) :
	       2'b00 ;
  assign wsiS_burstKind__EN =
	     WILL_FIRE_RL_wsiS_reqFifo_enq &&
	     (wsiS_burstKind == 2'b00 ||
	      wsiS_burstKind == 2'b01 && wsiS1_MReqLast ||
	      wsiS_burstKind == 2'b10 && wsiS1_MBurstLength[11:0] == 12'b000000000001) ;

  // register wsiS_errorSticky
  assign wsiS_errorSticky__D_IN = 1'b1 ;
  assign wsiS_errorSticky__EN =
	     WILL_FIRE_RL_wsiS_reqFifo_enq && !wsiS_reqFifo__FULL_N ;

  // register wsiS_iMesgCount
  assign wsiS_iMesgCount__D_IN = wsiS_iMesgCount + 32'b00000000000000000000000000000001 ;
  assign wsiS_iMesgCount__EN =
	     WILL_FIRE_RL_wsiS_reqFifo_enq && wsiS_burstKind == 2'b10 &&
	     wsiS1_MBurstLength[11:0] == 12'b000000000001 ;

  // register wsiS_operateD
  assign wsiS_operateD__D_IN = wci_cState == 3'b010 ;
  assign wsiS_operateD__EN = 1'b1 ;

  // register wsiS_pMesgCount
  assign wsiS_pMesgCount__D_IN = wsiS_pMesgCount + 32'b00000000000000000000000000000001 ;
  assign wsiS_pMesgCount__EN =
	     WILL_FIRE_RL_wsiS_reqFifo_enq && wsiS_burstKind == 2'b01 &&
	     wsiS1_MReqLast ;

  // register wsiS_peerIsReady
  assign wsiS_peerIsReady__D_IN = wsiS1_MReset_n ;
  assign wsiS_peerIsReady__EN = 1'b1 ;

  // register wsiS_reqFifo_countReg
  assign wsiS_reqFifo_countReg__D_IN =
	     CAN_FIRE_RL_wsiS_reqFifo_enq ?
	       wsiS_reqFifo_countReg + 2'b01 :
	       wsiS_reqFifo_countReg - 2'b01 ;
  assign wsiS_reqFifo_countReg__EN =
	     CAN_FIRE_RL_wsiS_reqFifo__updateLevelCounter ;

  // register wsiS_statusR
  assign wsiS_statusR__EN = 1'b1 ;
  assign wsiS_statusR__D_IN =
	     { wsiS_isReset__VAL,
	       !wsiS_peerIsReady,
	       !wsiS_operateD,
	       wsiS_errorSticky,
	       wsiS_burstKind != 2'b00,
	       NOT_wsiS_reqFifo_countReg_53_ULE_1_54___d355 ||
	       wsiS_isReset__VAL ||
	       !wsiS_operateD ||
	       !wsiS_peerIsReady,
	       1'b0,
	       wsiS_trafficSticky } ;

  // register wsiS_tBusyCount
  assign wsiS_tBusyCount__D_IN = wsiS_tBusyCount + 32'b00000000000000000000000000000001 ;
  assign wsiS_tBusyCount__EN = CAN_FIRE_RL_wsiS_inc_tBusyCount ;

  // register wsiS_trafficSticky
  assign wsiS_trafficSticky__D_IN = 1'b1 ;
  assign wsiS_trafficSticky__EN = CAN_FIRE_RL_wsiS_reqFifo_enq ;

  // register wsiWordsRemain
  assign wsiWordsRemain__D_IN =
	     MUX_mesgLength__write_1__SEL_2 ?
	       wsiS_reqFifo__D_OUT[55:44] :
	       MUX_wsiWordsRemain__write_1__VAL_2 ;
  assign wsiWordsRemain__EN =
	     WILL_FIRE_RL_wmwt_mesgBegin && wsiS_reqFifo__D_OUT[56] ||
	     WILL_FIRE_RL_wmwt_messagePushPrecise ;

  // register zeroLengthMesg
  assign zeroLengthMesg__D_IN = wsiS_reqFifo__D_OUT[11:8] == 4'b0000 ;
  assign zeroLengthMesg__EN = MUX_mesgLength__write_1__SEL_2 ;

  // submodule wci_reqF
  ////
  //assign wci_reqF__D_IN = wci_wciReq__wget ;
  //assign wci_reqF__ENQ = CAN_FIRE_RL_wci_reqF_enq ;
  //assign wci_reqF__DEQ = wci_reqF_r_deq__whas ;
  //assign wci_reqF__CLR = 1'b0 ;
  //assign wmi_respF__D_IN = wmi_wmiResponse__wget ;
  //assign wsiS_reqFifo__D_IN = wsiS_wsiReq__wget ;
  ////
  
  
  

  // submodule wmi_respF
  
  assign wmi_respF__DEQ = CAN_FIRE_RL_wmrd_mesgBodyResponse ;
  
  

  // submodule wsiS_reqFifo
    assign wsiS_reqFifo__ENQ = CAN_FIRE_RL_wsiS_reqFifo_enq ;
  ////
  //assign wsiS_reqFifo__CLR = 1'b0 ;
  ////
  assign wsiS_reqFifo__DEQ = wsiS_reqFifo_r_deq__whas ;
  assign wmi_respF__ENQ = CAN_FIRE_RL_wmi_respAdvance ;
  // remaining internal signals
  assign IF_mesgLength_22_BIT_14_23_THEN_mesgLength_22__ETC___d753 =
	     mesgLength[14] ? mesgLength[13:0] : 14'b00000000000000 ;
  assign NOT_wmi_reqF_c_r_46_EQ_2_47_48_AND_wmi_operate_ETC___d290 =
	     wmi_reqF_c_r != 2'b10 && wmi_operateD && wmi_peerIsReady &&
	     (!x__h18884 || wmi_mFlagF_c_r != 2'b10) ;
  assign NOT_wsiS_reqFifo_countReg_53_ULE_1_54___d355 =
	     wsiS_reqFifo_countReg > 2'b01 ;
  assign addr__h16647 = { mesgLengthSoFar[11:0], 2'b00 } ;
 // assign b__h13937 = -fabWordsCurReq[3:0] ;
   assign b__h13937 = fabWordsCurReq[3:0] ;
//  assign b__h19084 = { {10{fabRespCredit_value[3]}}, fabRespCredit_value } ;
assign b__h19084 = { {fabRespCredit_value[3],fabRespCredit_value[3],fabRespCredit_value[3],fabRespCredit_value[3],fabRespCredit_value[3],fabRespCredit_value[3],fabRespCredit_value[3],fabRespCredit_value[3],fabRespCredit_value[3],fabRespCredit_value[3]}, fabRespCredit_value } ;
  assign bl__h17580 =
	     zeroLengthMesg ?
	       12'b000000000001 :
	       IF_mesgLength_22_BIT_14_23_THEN_mesgLength_22__ETC___d753[13:2] ;
  assign mesgMetaF_length__h16810 = { 10'b0000000000, mlp1B__h16631 } ;
  assign mesgMetaF_length__h17723 =
	     { 10'b0000000000,
	       IF_mesgLength_22_BIT_14_23_THEN_mesgLength_22__ETC___d753 } ;
  assign mesgMetaF_opcode__h16809 = opcode[8] ? opcode[7:0] : 8'b00000000 ;
  assign mlp1B__h16631 = { mlp1__h16630[11:0], 2'b00 } ;
  assign mlp1__h16630 = mesgLengthSoFar + 14'b00000000000001 ;
  assign rdat__h15540 = { 16'b0000000000000000, x__h15543 } ;
  assign wsiBurstLength__h18454 =
	     smaCtrl[5] ? 16'b0000000000000010 : { 2'b00, thisMesg[15:2] } ;
  assign wsiS_reqFifo_i_notEmpty__52_AND_wmi_operateD_5_ETC___d165 =
	     wsiS_reqFifo__EMPTY_N && wmi_operateD && wmi_peerIsReady &&
	     (!x__h16715 || wmi_mFlagF_c_r != 2'b10) ;
  assign x__h15543 = { wsiS_statusR, wsiM_statusR } ;
  assign x__h16715 = wsiS_reqFifo__D_OUT[55:44] == 12'b000000000001 ;
  assign x__h18884 = fabWordsRemain == fabWordsCurReq ;
  assign x_burstLength__h18559 =
	     (thisMesg[15:0] == 16'b0000000000000000 || smaCtrl[5] && unrollCnt == 16'b0000000000000001) ?
	       12'b000000000001 :
	       (smaCtrl[5] ? 12'b111111111111 : wsiBurstLength__h18454[11:0]) ;
  assign x_byteEn__h18561 = (thisMesg[15:0] == 16'b0000000000000000) ? 4'b0000 : 4'b1111 ;
  assign x_length__h17087 =
	     { 2'b00,
	       IF_mesgLength_22_BIT_14_23_THEN_mesgLength_22__ETC___d753 } ;
  always@(wci_reqF__D_OUT or
	  smaCtrl or
	  mesgCount or
	  abortCount or
	  thisMesg or
	  lastMesg or
	  rdat__h15540 or wsiS_extStatusW__wget or wsiM_extStatusW__wget)
  begin
    case (wci_reqF__D_OUT[39:32])
      8'h0: x_data__h15447 = smaCtrl;
      8'h04: x_data__h15447 = mesgCount;
      8'h08: x_data__h15447 = abortCount;
      8'h10: x_data__h15447 = thisMesg;
      8'h14: x_data__h15447 = lastMesg;
      8'h18: x_data__h15447 = rdat__h15540;
      8'h20: x_data__h15447 = wsiS_extStatusW__wget[95:64];
      8'h24: x_data__h15447 = wsiS_extStatusW__wget[63:32];
      8'h28: x_data__h15447 = wsiS_extStatusW__wget[31:0];
      8'h2C: x_data__h15447 = wsiM_extStatusW__wget[95:64];
      8'h30: x_data__h15447 = wsiM_extStatusW__wget[63:32];
      8'h34: x_data__h15447 = wsiM_extStatusW__wget[31:0];
      default: x_data__h15447 = 32'b00000000000000000000000000000000;
    endcase
  end
  always@(MUX_endOfMessage__write_1__SEL_1 or
	  MUX_wmi_mFlagF_x_wire__wset_1__VAL_1 or
	  MUX_wmi_mFlagF_x_wire__wset_1__SEL_2 or
	  WILL_FIRE_RL_wmwt_requestPrecise or
	  MUX_wmi_mFlagF_x_wire__wset_1__VAL_3)
  begin
    case (1'b1) // synopsys parallel_case
      MUX_endOfMessage__write_1__SEL_1:
	  value__h6065 = MUX_wmi_mFlagF_x_wire__wset_1__VAL_1;
      MUX_wmi_mFlagF_x_wire__wset_1__SEL_2:
	  value__h6065 = 32'hAAAAAAAA /* unspecified value */ ;
      WILL_FIRE_RL_wmwt_requestPrecise:
	  value__h6065 = MUX_wmi_mFlagF_x_wire__wset_1__VAL_3;
      default: value__h6065 = 32'hAAAAAAAA /* unspecified value */ ;
    endcase
  end

  // handling of inlined registers

  always@(posedge wciS0_Clk)
  begin
    if (!wciS0_MReset_n)
      begin
        abortCount <=  32'b00000000000000000000000000000000;
	doAbort <=  1'b0;
	endOfMessage <=  1'b0;
	errCount <=  32'b00000000000000000000000000000000;
	fabRespCredit_value <=  4'b0000;
	fabWordsRemain <=  14'b00000000000000;
	firstMsgReq <=  1'b0;
	impreciseBurst <=  1'b0;
	lastMesg <=  32'hFEFEFFFE;
	mesgCount <=  32'b00000000000000000000000000000000;
	mesgLength <=  15'b010101010101010;
	mesgLengthSoFar <=  14'b00000000000000;
	mesgPreRequest <=  1'b0;
	mesgReqOK <=  1'b0;
	mesgReqValid <=  1'b0;
	opcode <=  9'b010101010;
	preciseBurst <=  1'b0;
	readyToPush <=  1'b0;
	readyToRequest <=  1'b0;
	smaCtrl <=  smaCtrlInit;
	thisMesg <=  32'hFEFEFFFE;
	unrollCnt <=  16'b0000000000000000;
	valExpect <=  32'b00000000000000000000000000000000;
	wci_cEdge <=  3'b111;
	wci_cState <=  3'b000;
	wci_ctlAckReg <=  1'b0;
	wci_ctlOpActive <=  1'b0;
	wci_illegalEdge <=  1'b0;
	wci_nState <=  3'b000;
	wci_reqF_countReg <=  2'b00;
	wci_respF_c_r <=  2'b00;
	wci_respF_q_0 <=  34'h0AAAAAAAA;
	wci_respF_q_1 <=  34'h0AAAAAAAA;
	wci_sFlagReg <=  1'b0;
	wci_sThreadBusy_d <=  1'b1;
	wmi_busyWithMessage <=  1'b0;
	wmi_dhF_c_r <=  2'b00;
	wmi_dhF_q_0 <=  38'h0AAAAAAAAA;
	wmi_dhF_q_1 <=  38'h0AAAAAAAAA;
	wmi_mFlagF_c_r <=  2'b00;
	wmi_mFlagF_q_0 <=  32'b00000000000000000000000000000000;
	wmi_mFlagF_q_1 <=  32'b00000000000000000000000000000000;
	wmi_operateD <=  1'b0;
	wmi_peerIsReady <=  1'b0;
	wmi_reqF_c_r <=  2'b00;
	wmi_reqF_q_0 <=  32'b00001010101010101010101010101010;
	wmi_reqF_q_1 <=  32'b00001010101010101010101010101010;
	wmi_sDataThreadBusy_d <=  1'b0;
	wmi_sFlagReg <=  32'b00000000000000000000000000000000;
	wmi_sThreadBusy_d <=  1'b0;
	wsiM_burstKind <=  2'b00;
	wsiM_errorSticky <=  1'b0;
	wsiM_iMesgCount <=  32'b00000000000000000000000000000000;
	wsiM_operateD <=  1'b0;
	wsiM_pMesgCount <=  32'b00000000000000000000000000000000;
	wsiM_peerIsReady <=  1'b0;
	wsiM_reqFifo_c_r <=  2'b00;
	wsiM_reqFifo_q_0 <=  61'h00000AAAAAAAAA00;
	wsiM_reqFifo_q_1 <=  61'h00000AAAAAAAAA00;
	wsiM_sThreadBusy_d <=  1'b1;
	wsiM_tBusyCount <=  32'b00000000000000000000000000000000;
	wsiM_trafficSticky <=  1'b0;
	wsiS_burstKind <=  2'b00;
	wsiS_errorSticky <=  1'b0;
	wsiS_iMesgCount <=  32'b00000000000000000000000000000000;
	wsiS_operateD <=  1'b0;
	wsiS_pMesgCount <=  32'b00000000000000000000000000000000;
	wsiS_peerIsReady <=  1'b0;
	wsiS_reqFifo_countReg <=  2'b00;
	wsiS_tBusyCount <=  32'b00000000000000000000000000000000;
	wsiS_trafficSticky <=  1'b0;
	wsiWordsRemain <=  12'b000000000000;
	zeroLengthMesg <=  1'b0;
      end
    else
      begin
        if (abortCount__EN)
	  abortCount <=  abortCount__D_IN;
	if (doAbort__EN) doAbort <=  doAbort__D_IN;
	if (endOfMessage__EN)
	  endOfMessage <=  endOfMessage__D_IN;
	if (errCount__EN) errCount <=  errCount__D_IN;
	if (fabRespCredit_value__EN)
	  fabRespCredit_value <=
	      fabRespCredit_value__D_IN;
	if (fabWordsRemain__EN)
	  fabWordsRemain <=  fabWordsRemain__D_IN;
	if (firstMsgReq__EN)
	  firstMsgReq <=  firstMsgReq__D_IN;
	if (impreciseBurst__EN)
	  impreciseBurst <=  impreciseBurst__D_IN;
	if (lastMesg__EN) lastMesg <=  lastMesg__D_IN;
	if (mesgCount__EN) mesgCount <=  mesgCount__D_IN;
	if (mesgLength__EN)
	  mesgLength <=  mesgLength__D_IN;
	if (mesgLengthSoFar__EN)
	  mesgLengthSoFar <=  mesgLengthSoFar__D_IN;
	if (mesgPreRequest__EN)
	  mesgPreRequest <=  mesgPreRequest__D_IN;
	if (mesgReqOK__EN) mesgReqOK <=  mesgReqOK__D_IN;
	if (mesgReqValid__EN)
	  mesgReqValid <=  mesgReqValid__D_IN;
	if (opcode__EN) opcode <=  opcode__D_IN;
	if (preciseBurst__EN)
	  preciseBurst <=  preciseBurst__D_IN;
	if (readyToPush__EN)
	  readyToPush <=  readyToPush__D_IN;
	if (readyToRequest__EN)
	  readyToRequest <=  readyToRequest__D_IN;
	if (smaCtrl__EN) smaCtrl <=  smaCtrl__D_IN;
	if (thisMesg__EN) thisMesg <=  thisMesg__D_IN;
	if (unrollCnt__EN) unrollCnt <=  unrollCnt__D_IN;
	if (valExpect__EN) valExpect <=  valExpect__D_IN;
	if (wci_cEdge__EN) wci_cEdge <=  wci_cEdge__D_IN;
	if (wci_cState__EN)
	  wci_cState <=  wci_cState__D_IN;
	if (wci_ctlAckReg__EN)
	  wci_ctlAckReg <=  wci_ctlAckReg__D_IN;
	if (wci_ctlOpActive__EN)
	  wci_ctlOpActive <=  wci_ctlOpActive__D_IN;
	if (wci_illegalEdge__EN)
	  wci_illegalEdge <=  wci_illegalEdge__D_IN;
	if (wci_nState__EN)
	  wci_nState <=  wci_nState__D_IN;
	if (wci_reqF_countReg__EN)
	  wci_reqF_countReg <=  wci_reqF_countReg__D_IN;
	if (wci_respF_c_r__EN)
	  wci_respF_c_r <=  wci_respF_c_r__D_IN;
	if (wci_respF_q_0__EN)
	  wci_respF_q_0 <=  wci_respF_q_0__D_IN;
	if (wci_respF_q_1__EN)
	  wci_respF_q_1 <=  wci_respF_q_1__D_IN;
	if (wci_sFlagReg__EN)
	  wci_sFlagReg <=  wci_sFlagReg__D_IN;
	if (wci_sThreadBusy_d__EN)
	  wci_sThreadBusy_d <=  wci_sThreadBusy_d__D_IN;
	if (wmi_busyWithMessage__EN)
	  wmi_busyWithMessage <=
	      wmi_busyWithMessage__D_IN;
	if (wmi_dhF_c_r__EN)
	  wmi_dhF_c_r <=  wmi_dhF_c_r__D_IN;
	if (wmi_dhF_q_0__EN)
	  wmi_dhF_q_0 <=  wmi_dhF_q_0__D_IN;
	if (wmi_dhF_q_1__EN)
	  wmi_dhF_q_1 <=  wmi_dhF_q_1__D_IN;
	if (wmi_mFlagF_c_r__EN)
	  wmi_mFlagF_c_r <=  wmi_mFlagF_c_r__D_IN;
	if (wmi_mFlagF_q_0__EN)
	  wmi_mFlagF_q_0 <=  wmi_mFlagF_q_0__D_IN;
	if (wmi_mFlagF_q_1__EN)
	  wmi_mFlagF_q_1 <=  wmi_mFlagF_q_1__D_IN;
	if (wmi_operateD__EN)
	  wmi_operateD <=  wmi_operateD__D_IN;
	if (wmi_peerIsReady__EN)
	  wmi_peerIsReady <=  wmi_peerIsReady__D_IN;
	if (wmi_reqF_c_r__EN)
	  wmi_reqF_c_r <=  wmi_reqF_c_r__D_IN;
	if (wmi_reqF_q_0__EN)
	  wmi_reqF_q_0 <=  wmi_reqF_q_0__D_IN;
	if (wmi_reqF_q_1__EN)
	  wmi_reqF_q_1 <=  wmi_reqF_q_1__D_IN;
	if (wmi_sDataThreadBusy_d__EN)
	  wmi_sDataThreadBusy_d <=
	      wmi_sDataThreadBusy_d__D_IN;
	if (wmi_sFlagReg__EN)
	  wmi_sFlagReg <=  wmi_sFlagReg__D_IN;
	if (wmi_sThreadBusy_d__EN)
	  wmi_sThreadBusy_d <=  wmi_sThreadBusy_d__D_IN;
	if (wsiM_burstKind__EN)
	  wsiM_burstKind <=  wsiM_burstKind__D_IN;
	if (wsiM_errorSticky__EN)
	  wsiM_errorSticky <=  wsiM_errorSticky__D_IN;
	if (wsiM_iMesgCount__EN)
	  wsiM_iMesgCount <=  wsiM_iMesgCount__D_IN;
	if (wsiM_operateD__EN)
	  wsiM_operateD <=  wsiM_operateD__D_IN;
	if (wsiM_pMesgCount__EN)
	  wsiM_pMesgCount <=  wsiM_pMesgCount__D_IN;
	if (wsiM_peerIsReady__EN)
	  wsiM_peerIsReady <=  wsiM_peerIsReady__D_IN;
	if (wsiM_reqFifo_c_r__EN)
	  wsiM_reqFifo_c_r <=  wsiM_reqFifo_c_r__D_IN;
	if (wsiM_reqFifo_q_0__EN)
	  wsiM_reqFifo_q_0 <=  wsiM_reqFifo_q_0__D_IN;
	if (wsiM_reqFifo_q_1__EN)
	  wsiM_reqFifo_q_1 <=  wsiM_reqFifo_q_1__D_IN;
	if (wsiM_sThreadBusy_d__EN)
	  wsiM_sThreadBusy_d <=  wsiM_sThreadBusy_d__D_IN;
	if (wsiM_tBusyCount__EN)
	  wsiM_tBusyCount <=  wsiM_tBusyCount__D_IN;
	if (wsiM_trafficSticky__EN)
	  wsiM_trafficSticky <=  wsiM_trafficSticky__D_IN;
	if (wsiS_burstKind__EN)
	  wsiS_burstKind <=  wsiS_burstKind__D_IN;
	if (wsiS_errorSticky__EN)
	  wsiS_errorSticky <=  wsiS_errorSticky__D_IN;
	if (wsiS_iMesgCount__EN)
	  wsiS_iMesgCount <=  wsiS_iMesgCount__D_IN;
	if (wsiS_operateD__EN)
	  wsiS_operateD <=  wsiS_operateD__D_IN;
	if (wsiS_pMesgCount__EN)
	  wsiS_pMesgCount <=  wsiS_pMesgCount__D_IN;
	if (wsiS_peerIsReady__EN)
	  wsiS_peerIsReady <=  wsiS_peerIsReady__D_IN;
	if (wsiS_reqFifo_countReg__EN)
	  wsiS_reqFifo_countReg <=
	      wsiS_reqFifo_countReg__D_IN;
	if (wsiS_tBusyCount__EN)
	  wsiS_tBusyCount <=  wsiS_tBusyCount__D_IN;
	if (wsiS_trafficSticky__EN)
	  wsiS_trafficSticky <=  wsiS_trafficSticky__D_IN;
	if (wsiWordsRemain__EN)
	  wsiWordsRemain <=  wsiWordsRemain__D_IN;
	if (zeroLengthMesg__EN)
	  zeroLengthMesg <=  zeroLengthMesg__D_IN;
      end
    if (fabWordsCurReq__EN)
      fabWordsCurReq <=  fabWordsCurReq__D_IN;
    if (mesgReqAddr__EN) mesgReqAddr <=  mesgReqAddr__D_IN;
    if (wsiM_statusR__EN)
      wsiM_statusR <=  wsiM_statusR__D_IN;
    if (wsiS_statusR__EN)
      wsiS_statusR <=  wsiS_statusR__D_IN;
  end


endmodule  // mkSMAdapter4B



module ResetToBool (RST, VAL);

input RST;
output VAL;
reg VAL;

always @ (RST or VAL)
begin

if (RST == 1)
VAL=1'b0;

end
endmodule