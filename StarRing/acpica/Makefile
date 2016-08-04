


FILE += debugger/dbcmds.o debugger/dbconvert.o debugger/dbdisply.o debugger/dbexec.o debugger/dbfileio.o debugger/dbhistry.o debugger/dbinput.o debugger/dbmethod.o debugger/dbnames.o debugger/dbstats.o debugger/dbtest.o debugger/dbutils.o debugger/dbxface.o 
FILE += disassembler/dmbuffer.o disassembler/dmdeferred.o disassembler/dmnames.o disassembler/dmobject.o disassembler/dmopcode.o disassembler/dmresrc.o disassembler/dmresrcl.o disassembler/dmresrcl2.o disassembler/dmresrcs.o disassembler/dmutils.o disassembler/dmwalk.o 
FILE += dispatcher/dsargs.o dispatcher/dscontrol.o dispatcher/dsfield.o dispatcher/dsinit.o dispatcher/dsmethod.o dispatcher/dsmthdat.o dispatcher/dsobject.o dispatcher/dsopcode.o dispatcher/dsutils.o dispatcher/dswexec.o dispatcher/dswload.o dispatcher/dswload2.o dispatcher/dswscope.o dispatcher/dswstate.o 
FILE += events/evevent.o events/evglock.o events/evgpe.o events/evgpeblk.o events/evgpeinit.o events/evgpeutil.o events/evhandler.o events/evmisc.o events/evregion.o events/evrgnini.o events/evsci.o events/evxface.o events/evxfevnt.o events/evxfgpe.o events/evxfregn.o 
FILE += executer/exconfig.o executer/exconvrt.o executer/excreate.o executer/exdebug.o executer/exdump.o executer/exfield.o executer/exfldio.o executer/exmisc.o executer/exmutex.o executer/exnames.o executer/exoparg1.o executer/exoparg2.o executer/exoparg3.o executer/exoparg6.o executer/exprep.o executer/exregion.o executer/exresnte.o executer/exresolv.o executer/exresop.o executer/exstore.o executer/exstoren.o executer/exstorob.o executer/exsystem.o executer/exutils.o 
FILE += hardware/hwacpi.o hardware/hwesleep.o hardware/hwgpe.o hardware/hwpci.o hardware/hwregs.o hardware/hwsleep.o hardware/hwtimer.o hardware/hwvalid.o hardware/hwxface.o hardware/hwxfsleep.o 
FILE += namespace/nsaccess.o namespace/nsalloc.o namespace/nsarguments.o namespace/nsconvert.o namespace/nsdump.o namespace/nsdumpdv.o namespace/nseval.o namespace/nsinit.o namespace/nsload.o namespace/nsnames.o namespace/nsobject.o namespace/nsparse.o namespace/nspredef.o namespace/nsprepkg.o namespace/nsrepair.o namespace/nsrepair2.o namespace/nssearch.o namespace/nsutils.o namespace/nswalk.o namespace/nsxfeval.o namespace/nsxfname.o namespace/nsxfobj.o 
FILE += parser/psargs.o parser/psloop.o parser/psobject.o parser/psopcode.o parser/psopinfo.o parser/psparse.o parser/psscope.o parser/pstree.o parser/psutils.o parser/pswalk.o parser/psxface.o 
FILE += resources/rsaddr.o resources/rscalc.o resources/rscreate.o resources/rsdump.o resources/rsdumpinfo.o resources/rsinfo.o resources/rsio.o resources/rsirq.o resources/rslist.o resources/rsmemory.o resources/rsmisc.o resources/rsserial.o resources/rsutils.o resources/rsxface.o 
FILE += tables/tbdata.o tables/tbfadt.o tables/tbfind.o tables/tbinstal.o tables/tbprint.o tables/tbutils.o tables/tbxface.o tables/tbxfload.o tables/tbxfroot.o 
FILE += utilities/utaddress.o utilities/utalloc.o utilities/utbuffer.o utilities/utcache.o utilities/utclib.o utilities/utcopy.o utilities/utdebug.o utilities/utdecode.o utilities/utdelete.o utilities/uterror.o utilities/uteval.o utilities/utexcep.o utilities/utfileio.o utilities/utglobal.o utilities/uthex.o utilities/utids.o utilities/utinit.o utilities/utlock.o utilities/utmath.o utilities/utmisc.o utilities/utmutex.o utilities/utobject.o utilities/utosi.o utilities/utownerid.o utilities/utpredef.o utilities/utprint.o utilities/utresrc.o utilities/utstate.o utilities/utstring.o utilities/uttrack.o utilities/utuuid.o utilities/utxface.o utilities/utxferror.o utilities/utxfinit.o utilities/utxfmutex.o 

TARGET := acpica.sys


.c.o:
	$(CC) -mcmodel=kernel -I./include -include ./include/platform/acstarring.h -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

.S.o:
	$(CC) -mcmodel=kernel -I../$(INCLUDE) -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

#コンパイル
compile: $(TARGET)


$(TARGET):$(FILE)

	$(LD) -r -Map $(TARGET).map -o $(TARGET) $(FILE)


#バイナリ消去
clean:
	#$(RM) -f $(FILE) $(TARGET)
























