JMF 2.1.1e Software
[ http://java.sun.com/javase/technologies/desktop/media/jmf/2.1.1/download.html ]
 Download Java(TM) Media Framework (JMF) 2.1.1e for Linux
  Linux Performance Pack: jmf-2_1_1e-linux-i586.bin

JMF MP3 Plugin
[ http://java.sun.com/javase/technologies/desktop/media/jmf/mp3/download.html ]
 Download Java MP3 Plugin 1.0 FCS for Other 
  Java MP3 Plugin: javamp3-1_0.zip

Instructions:
 Copy 'jmf-2_1_1e-linux-i586.bin' to 'LIBs' directory.
 - Caso esteja no ubuntu, modifique o .bin com o vim.
 $ vim -b jmf-2_1_1e-linux-i586.bin
 - Modifique na linha 267 [ http://wiki.services.openoffice.org/wiki/Java/Java_Media_Framework#Troubleshooting ]
   'tail +309 $0 > $outname' por 'tail -n +309 $0 > $outname'
 $ chmod a+x jmf-2_1_1e-linux-i586.bin 
 $ ./jmf-2_1_1e-linux-i586.bin
 - tecle 'q' para sair do 'Binary Code License Agreement' (more)
 - tecle 'yes' or 'no' para as perguntas de config.

 - Caso tudo tenha dado certo voce terá uma sequencia de pastas assim: 'LIBs/JMF-2.1.1e/lib/jmf.jar' para o arquivo 'jmf.jar'

 - Variáveis auxiliares de ambiente
 $ export JMFHOME=../LIBs/JMF-2.1.1e
 $ export CLASSPATH=.:$CLASSPATH:$JMFHOME/lib/jmf.jar
 $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JMFHOME/lib

 - Para erros de janela, modifique o AWT Toolkit padrão.
 $ java -Dawt.toolkit=sun.awt.motif.MToolkit JMStudio

 - Java MP3 Lib
 $ mkdir javamp3
 $ unzip javamp3-1_0.zip -d javamp3/
 $ cp javamp3/lib/ext/mp3plugin.jar JMF-2.1.1e/lib/

