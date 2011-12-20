
SUBTARGETS=caseadmin ocfahalt
all:
	make -C caseadmin all
	make -C caseadmin/priv all
	make -C ocfahalt all


install:	
	make -C caseadmin install
	make -C caseadmin/priv install
	make -C ocfahalt install	

clean:
	make -C caseadmin clean
	make -C ocfahalt clean

dpkg:
	echo No make dpkg implemented for admin.
	exit 1
