��    G      T  a   �        o     ?   �  �   �  .   X  #   �     �  '   �     �     �          +  (   :     c  K   z     �     �     �  -   �     	     ,	     4	     B	  8   Y	  M   �	  k   �	  8   L
  (   �
     �
     �
  u   �
     H     M  X   R  @   �     �       ;     6   [  7   �  �   �  /   S  4   �  =   �  Y   �  �  P  )     7   >     v  1   �  '   �  .   �  C     F  b     �  �   �     D     J  n   j     �  @   �     3  &   P     w     z  '   �     �  !   �     �  a        m  �  q  o     9   �  �   �  ;   q  0   �     �  (   �  
   #     .     E     [  #   j     �  M   �     �     �       0         Q     _     g     v  K   �  Y   �  �   4  <   �  !   �       %     g   B     �     �  c   �  D        _     l  @   �  G   �  6        K  +   �  9   �  >   1  J   p  �  �  1   �!  C   �!  1   �!  )   /"  /   Y"  -   �"  <   �"  a  �"     V$  �   t$     %  "   %  q   2%     �%  9   �%  "   �%  4    &     U&     Y&  +   k&     �&  0   �&  0   �&  n   '     |'        4      '       B                    >                       0      A                    ?       *                  (       3   =   ,   ;                 8   /   <   6               -      E   .   C           9       1                            +   2   #          D   G         :      %      7          !       $   "   )   
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
       --no-reload		Do not reload templates. (Use with caution.) Valid priorities are: %s You are using the editor-based debconf frontend to configure your system. See the end of this document for detailed instructions. _Help apt-extracttemplates failed: %s debconf-mergetemplate: This utility is deprecated. You should switch to using po-debconf's po2debconf program. debconf: can't chmod: %s delaying package configuration, since apt-utils is not installed falling back to frontend: %s must specify some debs to preconfigure no none of the above please specify a package to reconfigure template parse error: %s unable to initialize frontend: %s unable to re-open stdin: %s warning: possible database corruption. Will attempt to repair by adding back missing question %s. yes Project-Id-Version: nb
Report-Msgid-Bugs-To: 
POT-Creation-Date: 2012-03-15 13:21+0000
PO-Revision-Date: 2012-01-05 22:20+0100
Last-Translator: Bjørn Steensrud <bjornst@skogkatt.homelinux.org>
Language-Team: Norwegian Bokmål <i18n-nb@lister.ping.uio.no>
Language: nb
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Generator: Lokalize 1.2
Plural-Forms:  nplurals=2; plural=(n != 1);
 
        --outdated		Flett inn også utdaterte oversettelser.
	--drop-old-templates	Dropp hele utdaterte maler. 
  -o,  --owner=package		Sett pakken som eier kommandoen.   -f,  --frontend		Oppgi debconf grensesnitt som skal brukes.
  -p,  --priority		Oppgi minste prioritet for spørsmål som skal vises.
       --terse			Vær ordknapp.
 forhåndsoppsettet av %s mislyktes med avslutningsstatus %s %s er ødelagt eller ikke fullstendig installert %s er uklar ved byte %s: %s %s er uklar ved byte %s: %s; dropper den %s mangler %s mangler: dropper %s %s er ikke installert %s er utdatert %s er utdatert; dropper hele malen! %s må kjøres som root (Skriv inn null eller flere elementet atskilt med komma og mellomrom (', ').) Tilbake Klarer ikke lese statusfil: %s Valg Oppsettsdatabase er ikke oppgitt i oppsettsfila. Setter opp %s Debconf Debconf på %s Debconf, kjører på %s Det dialogbaserte grensesnittet er ikke kompatibelt med emacs skall-buffere Det dialogbaserte grensesnittet krever et skjermvindu på minst 13 linjer og 31 kolonner. Det dialogbaserte grensesnittet vil ikke fungere på en dum terminal, en emacs skall-buffer eller uten en kontrollerende terminal. Skriv inn de elementene du vil velge, atskilt med mellomrom. Trekker ut maler fra pakker: %d%% Hjelp Ignorerer ugyldig prioritering «%s» Inngangsverdi, «%s» ikke funnet i C-valgene! Dette skal aldri hende. Kanskje malen var plassert feil. Flere Neste Ingen brukbare dialogprogrammer er installert, så det dialogbaserte grensesnittet kan ikke brukes. Merk: Debconf kjører i nettlesermodus. Gå til http://localhost:%i/ Pakkeoppsett Forhåndsoppsetter pakker ...
 Problem med å sette opp databasen definert av strofe %s fra %s. TERM er ikke satt, så det dialogbaserte grensesnittet kan ikke brukes. Mal #%s i %s inneholder ingen linje med «Template:»
 Mal #%s i %s har et duplisert felt «%s» med ny verdi «%s». Antakelig er to maler ikke skikkelig atskilte med en tom linje.
 Maldatabase er ikke oppgitt i oppsettsfila. Maltolkingsfeil i nærheten av «%s», i strofe #%s i %s
 Term::ReadLine::GNU er ikke kompatibel med emacs skall-buffer. Valgene Sigils og Smileys er ikke i bruk i oppsettsfila lenger. Fjern dem. Det redigeringsbaserte debconf-grensesnittet viser deg en eller fleretekstfiler som skal redigeres. Dette er en slik tekstfil. Hvis du er vant med standard unix oppsettsfiler, vil denne fila se kjent ut for deg. Den inneholder kommentarer innimellom oppsettselementene. Rediger fila, endre de elementene som trengs, lagre den og avslutt. Ved det tidspunktet vil debconf lese den redigerte fila og bruke de verdiene du har skrevet inn for å setteopp systemet. Dette grensesnittet krever en kontrollerende tty. Ikke i stand til å laste Debconf::Element::%s. Mislyktes fordi: %s Ikke i stand til å starte opp et grensesnitt: %s Ukjent malfelt «%s», i strofe #%s i %s
 Bruk: debconf [parametre] kommando [argumenter] Bruk: debconf-communicate [parametre] [pakke] Bruk: debconf-mergetemplate [parametre] [maler.ll ...] maler Bruk: dpkg-reconfigure [options] pakker
  -a,  --all			Sett opp alle pakker på nytt.
  -u,  --unseen-only		Vis bare spørsmål som ikke er sett ennå.
       --default-priority	Bruk standard prioritet i stedet for lav.
       --force			Nytt oppsett påtvinges ødelagte pakker.
       --no-reload		Ikke last inn maler på nytt. (Bruk med forsiktighet.) Gyldige prioriteringer er: %s Du bruker det redigeringsbaserte debconf-grensesnittet for å sette opp systemet ditt. Se slutten av dette dokumentet for detaljerte instruksjoner. _Hjelp apt-extracttemplates mislyktes: %s debconf-mergetemplate: Dette verktøyet frarådes. Du bør bytte til å bruke po2debconf-programmet i po-debconf. debconf: klarer ikke chmod: %s utsetter pakkeoppsett, siden apt-utils ikke er installert går tilbake til grensesnittet: %s må oppgi noen debs som skal settes opp på forhånd nei ingen av dem over oppgi en pakke som skal settes opp på nytt maltolkingsfeil: %s ikke i stand til å starte opp grensesnittet: %s ikke i stand til å åpne standard inn igjen: %s advarsel: muligens ødelagt database. Vil forsøke å reparere ved å legge til igjen manglende spørsmål %s. ja 