��    G      T  a   �        o     ?   �  �   �  .   X  #   �     �  '   �     �     �          +  (   :     c  K   z     �     �     �  -   �     	     ,	     4	     B	  8   Y	  M   �	  k   �	  8   L
  (   �
     �
     �
  u   �
     H     M  X   R  @   �     �       ;     6   [  7   �  �   �  /   S  4   �  =   �  Y   �  �  P  )     7   >     v  1   �  '   �  .   �  C     F  b     �  �   �     D     J  n   j     �  @   �     3  &   P     w     z  '   �     �  !   �     �  a        m  �  q  �   E  @   �  �     3   �  #   �     
  ,   '     T     d     z     �  9   �     �  Z   �     U  '   \     �  S   �     �     �     �       E   $  \   j  �   �  B   L  -   �     �  #   �  ~   �     e  	   j  q   t  K   �     2     I  M   g  G   �  7   �  �   5  L   �  @   *   @   k   e   �   	  !  %   #  6   B#  !   y#  9   �#  ,   �#  -   $  C   0$  S  t$     �%  �   �%     t&     z&  g   �&  +   '  J   .'     y'  /   �'     �'     �'  /   �'     (     1(     P(  k   m(     �(        4      '       B                    >                       0      A                    ?       *                  (       3   =   ,   ;                 8   /   <   6               -      E   .   C           9       1                            +   2   #          D   G         :      %      7          !       $   "   )   
      F      	   5   @   &    
        --outdated		Merge in even outdated translations.
	--drop-old-templates	Drop entire outdated templates. 
  -o,  --owner=package		Set the package that owns the command.   -f,  --frontend		Specify debconf frontend to use.
  -p,  --priority		Specify minimum priority question to show.
       --terse			Enable terse mode.
 %s failed to preconfigure, with exit status %s %s is broken or not fully installed %s is fuzzy at byte %s: %s %s is fuzzy at byte %s: %s; dropping it %s is missing %s is missing; dropping %s %s is not installed %s is outdated %s is outdated; dropping whole template! %s must be run as root (Enter zero or more items separated by a comma followed by a space (', ').) Back Cannot read status file: %s Choices Config database not specified in config file. Configuring %s Debconf Debconf on %s Debconf, running at %s Dialog frontend is incompatible with emacs shell buffers Dialog frontend requires a screen at least 13 lines tall and 31 columns wide. Dialog frontend will not work on a dumb terminal, an emacs shell buffer, or without a controlling terminal. Enter the items you want to select, separated by spaces. Extracting templates from packages: %d%% Help Ignoring invalid priority "%s" Input value, "%s" not found in C choices! This should never happen. Perhaps the templates were incorrectly localized. More Next No usable dialog-like program is installed, so the dialog based frontend cannot be used. Note: Debconf is running in web mode. Go to http://localhost:%i/ Package configuration Preconfiguring packages ...
 Problem setting up the database defined by stanza %s of %s. TERM is not set, so the dialog frontend is not usable. Template #%s in %s does not contain a 'Template:' line
 Template #%s in %s has a duplicate field "%s" with new value "%s". Probably two templates are not properly separated by a lone newline.
 Template database not specified in config file. Template parse error near `%s', in stanza #%s of %s
 Term::ReadLine::GNU is incompatable with emacs shell buffers. The Sigils and Smileys options in the config file are no longer used. Please remove them. The editor-based debconf frontend presents you with one or more text files to edit. This is one such text file. If you are familiar with standard unix configuration files, this file will look familiar to you -- it contains comments interspersed with configuration items. Edit the file, changing any items as necessary, and then save it and exit. At that point, debconf will read the edited file, and use the values you entered to configure the system. This frontend requires a controlling tty. Unable to load Debconf::Element::%s. Failed because: %s Unable to start a frontend: %s Unknown template field '%s', in stanza #%s of %s
 Usage: debconf [options] command [args] Usage: debconf-communicate [options] [package] Usage: debconf-mergetemplate [options] [templates.ll ...] templates Usage: dpkg-reconfigure [options] packages
  -a,  --all			Reconfigure all packages.
  -u,  --unseen-only		Show only not yet seen questions.
       --default-priority	Use default priority instead of low.
       --force			Force reconfiguration of broken packages.
       --no-reload		Do not reload templates. (Use with caution.) Valid priorities are: %s You are using the editor-based debconf frontend to configure your system. See the end of this document for detailed instructions. _Help apt-extracttemplates failed: %s debconf-mergetemplate: This utility is deprecated. You should switch to using po-debconf's po2debconf program. debconf: can't chmod: %s delaying package configuration, since apt-utils is not installed falling back to frontend: %s must specify some debs to preconfigure no none of the above please specify a package to reconfigure template parse error: %s unable to initialize frontend: %s unable to re-open stdin: %s warning: possible database corruption. Will attempt to repair by adding back missing question %s. yes Project-Id-Version: debconf
Report-Msgid-Bugs-To: 
POT-Creation-Date: 2012-03-15 13:21+0000
PO-Revision-Date: 2010-03-08 18:00+0200
Last-Translator: Xandru Armesto <xandru@softastur.org>
Language-Team: Asturian Team <alministradores@softastur.org>
Language: ast
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
Plural-Forms: nplurals=2; plural=n != 1;
X-Generator: Virtaal 0.5.2
X-Launchpad-Export-Date: 2009-02-17 13:28+0000
 
        --outdated		Amestar tamién tornes desactualizaes.
	--drop-old-templates	Descartar completamente les plantilles desactualizaes. 
  -o,  --owner=paquete		Afita'l dueñu del paquete por comandu.   -f,  --frontend		Conseña la interfaz a usar por debconf.
  -p,  --priority		Especifica la prioridá mínima a mostrar.
       --terse			Activa'l mou resumíu.
 falló al preconfigurar %s, con estáu de salida %s %s ta rotu o non instaláu ensembre %s ta difusa nel byte %s: %s %s ta difusa nel byte %s: %s; descartándola ye requeríu %s falta %s: inorando %s %s nun ta instaláu %s ta desactualizáu %s ta desactualizada: ¡descartando la plantía completa! %s debes executalu como root (Introduz dengún o más elementos dixebraos por una coma siguíos per un espaciu (', ').) Volver Nun puede lleese'l ficheru d'estáu: %s Opciones Nun s'especificó configuración de la base de datos nel ficheru de configuración. Configuración de %s Debconf Debconf en %s Debconf, corriendo a %s El frontend de diálogu nun ye compatible colos buffers shell d'emacs Frontend dialog requier una pantalla con al menos 13 llinies d'altor y 31 columnes d'anchor. La interfaz dialog nun trabayará nun terminal tontu, un buffer d'intérprete d'órdenes d'emacs, o ensin una terminal controladora. Introduz l'elementu que quies seleicionar, separtáu per espacios. Estrayendo plantíes dende los paquetes: %d%% Aida Inorando prioridá nun válida "%s" ¡Valor d'entrada, "%s" non atopáu n'opciones C! Esto enxamás debiera pasar. Seique les plantíes nun tán bien llocalizaes. Más Siguiente Nun hai dengún programa de la triba dialog instaláu, asina que nun se puede usar la interface basada en dialog. Ten en cuenta: Debconf ta corriendo en mou web. Vete a http://localhost:%i/ Configuración paquete Preconfigurando paquetes ...
 Hebo un fallu configurando la base de datos denifida pola instancia %s de %s. TÉRMINU nun ta afitáu, colo que'l diálogu de frontend nun ye usable. Plantía #%s en %s nun contién una llinia 'Template:'
 La plantía #%s en %s tien un campu «%s» duplicáu col nuevu valor «%s». Dablemente dos plantíes nun tán dixebraes correutamente con ún sólo retornu de carru.
 Plantía de la base de datos nun especificada nel ficheru de configuración. Fallu d'analís de plantía cerca de `%s', na estrofa #%s de %s
 Term::ReadLine::GNU ye incompatible colos buffers shell d'emacs. Yá nun s'usen les opciones Sigils y Smileys nel ficheru de configuración. Por favor, desaníciales. La interface de debconf basada nel editor amuesa ún o más ficheros de testu pa que los igües. Ésti ye ún d'esos ficheros de testu. Si tas familiarizáu colos ficheros de configuración estándar d'Unix, esti ficheru resultaráte familiar; contién comentarios intercalaos con elementos de configuración. Igua esti ficheru, camudando cualisquier elementu según seya necesariu, y lluéu grábalu y sal del editor. Nesi puntu, debconf lleerá'l ficheru iguáu, y usará los valores inxertaos pa configurar el sistema. Esti frontend requier un control tty. Nun puede cargase Debconf::Element::%s. Falló por: %s Nun pudo aniciase un frontend: %s Campu desconocíu '%s' na plantía, na estrofa #%s de %s
 Usu: debconf [opciones] comandu [argumentos] Usu: debconf-communicate [opciones] [paquete] Usu: debconf-mergetemplate [opciones] [plantilles.ll ...] plantíes Usu: dpkg-reconfigure [opciones] paquetes
  -a,  --all			Reconfigura tolos paquetes.
  -u,  --unseen-only		Amosar namás entrugues nun vistes tovía.
       --default-priority	Usa prioridá por defeutu a la baxa.
       --force			Forciar reconfiguración de paquetes frayaos.
       --no-reload		Nun recargar plantíes. (Usar con curiáu.) Les prioridaes válides son: %s Tas usando un editor basáu nel frontend debconf pa configurar el to sistema. Mira a lo cabero d'esti documentu por intrucciones detallaes. _Aida falló apt-extracttemplates: %s debconf-mergetemplate: Esta utilidá ye obsoleta. Tendría d'usar el programa de po-debconf po2debconf. debconf: nun puede camudar los permisos: %s retrasando configuración del paquete, dende qu'apt-utils nun ta instaláu probando agora'l frontend: %s debes especificar dalgún debs pa preconfigurar non dengún de los d'enriba por favor, especifica un paquete a reconfigurar Fallu procesando plantía: %s nun pudo aniciase frontend: %s nun puede reabrise stdin: %s avisu: dable corrupción de la base de datos. Intentará iguase volviendo a amesta-y la entruga perdida %s. sí 