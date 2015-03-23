;Language: Brazilian Portuguese (1046)
;By Diego Marcos <jump@osite.com.br>

!insertmacro LANGFILE "PortugueseBR" "Portugu�s (Brasil)"

!ifdef MUI_WELCOMEPAGE
  ${LangFileString} MUI_TEXT_WELCOME_INFO_TITLE "Bem-vindo ao Assistente de Instala��o do $(^NameDA)"
  ${LangFileString} MUI_TEXT_WELCOME_INFO_TEXT "Este assistente o guiar� durante a instala��o do $(^NameDA).$\r$\n$\r$\n� recomendado fechar todas as outras aplica��es antes de iniciar a instala��o. Isto possibilitar� atualizar os arquivos de sistema relevantes sem reiniciar o computador.$\r$\n$\r$\n$_CLICK"
!endif

!ifdef MUI_UNWELCOMEPAGE
  ${LangFileString} MUI_UNTEXT_WELCOME_INFO_TITLE "Bem-vindo ao Assistente de Desinstala��o do $(^NameDA)"
  ${LangFileString} MUI_UNTEXT_WELCOME_INFO_TEXT "Este assistente o guiar� durante a desinstala��o do $(^NameDA).$\r$\n$\r$\nAntes de iniciar a desinstala��o, tenha certeza que o $(^NameDA) n�o est� sendo executado.$\r$\n$\r$\n$_CLICK"
!endif

!ifdef MUI_LICENSEPAGE
  ${LangFileString} MUI_TEXT_LICENSE_TITLE "Acordo de licen�a"
  ${LangFileString} MUI_TEXT_LICENSE_SUBTITLE "Por favor, reveja os termos do acordo antes de instalar o $(^NameDA)."
  ${LangFileString} MUI_INNERTEXT_LICENSE_BOTTOM "Se voc� concorda com os termos do acordo, clique em Concordar para continuar. Voc� deve aceitar o acordo para instalar o $(^NameDA)."
  ${LangFileString} MUI_INNERTEXT_LICENSE_BOTTOM_CHECKBOX "Se voc� concorda com termos do acordo, clique na caixa de sele��o abaixo. Voc� deve aceitar o acordo para instalar o $(^NameDA). $_CLICK"
  ${LangFileString} MUI_INNERTEXT_LICENSE_BOTTOM_RADIOBUTTONS "Se voc� concorda com termos do acordo, selecione a primeira op��o abaixo. Voc� deve aceitar o acordo para instalar o $(^NameDA). $_CLICK"
!endif

!ifdef MUI_UNLICENSEPAGE
  ${LangFileString} MUI_UNTEXT_LICENSE_TITLE "Acordo de licen�a"
  ${LangFileString} MUI_UNTEXT_LICENSE_SUBTITLE "Por favor, reveja os termos do acordo antes de desinstalar o $(^NameDA)."
  ${LangFileString} MUI_UNINNERTEXT_LICENSE_BOTTOM "Se voc� concorda com os termos do acordo, clique em Concordar para continuar. Voc� deve aceitar o acordo para desinstalar o $(^NameDA)."
  ${LangFileString} MUI_UNINNERTEXT_LICENSE_BOTTOM_CHECKBOX "Se voc� concorda com os termos do acordo, clique na caixa de sele��o abaixo. Voc� deve aceitar o acordo para desinstalar o $(^NameDA). $_CLICK"
  ${LangFileString} MUI_UNINNERTEXT_LICENSE_BOTTOM_RADIOBUTTONS "Se voc� concorda com os termos do acordo, selecione a primeira op��o abaixo. Voc� deve aceitar o acordo para desinstalar o $(^NameDA). $_CLICK"
!endif

!ifdef MUI_LICENSEPAGE | MUI_UNLICENSEPAGE
  ${LangFileString} MUI_INNERTEXT_LICENSE_TOP "Tecle Page Down para ver o restante do acordo."
!endif

!ifdef MUI_COMPONENTSPAGE
  ${LangFileString} MUI_TEXT_COMPONENTS_TITLE "Escolher componentes"
  ${LangFileString} MUI_TEXT_COMPONENTS_SUBTITLE "Escolha quais fun��es do $(^NameDA) que deseja instalar."
  ${LangFileString} MUI_INNERTEXT_COMPONENTS_DESCRIPTION_TITLE "Descri��o"
!endif

!ifdef MUI_UNCOMPONENTSPAGE
  ${LangFileString} MUI_UNTEXT_COMPONENTS_TITLE "Escolher componentes"
  ${LangFileString} MUI_UNTEXT_COMPONENTS_SUBTITLE "Escolha quais fun��es do $(^NameDA) que deseja desinstalar."
!endif

!ifdef MUI_COMPONENTSPAGE | MUI_UNCOMPONENTSPAGE
  !ifndef NSIS_CONFIG_COMPONENTPAGE_ALTERNATIVE
    ${LangFileString} MUI_INNERTEXT_COMPONENTS_DESCRIPTION_INFO "Posicione a seta do mouse sobre um componente para ver sua descri��o."
  !else
    ${LangFileString} MUI_INNERTEXT_COMPONENTS_DESCRIPTION_INFO "Posicione a seta do mouse sobre um componente para ver sua descri��o."
  !endif
!endif

!ifdef MUI_DIRECTORYPAGE
  ${LangFileString} MUI_TEXT_DIRECTORY_TITLE "Escolher o local da instala��o"
  ${LangFileString} MUI_TEXT_DIRECTORY_SUBTITLE "Escolha a pasta na qual deseja instalar o $(^NameDA)."
!endif

!ifdef MUI_UNDIRECTORYSPAGE
  ${LangFileString} MUI_UNTEXT_DIRECTORY_TITLE "Escolher o local da desinstala��o"
  ${LangFileString} MUI_UNTEXT_DIRECTORY_SUBTITLE "Escolha a pasta na qual deseja desinstalar o $(^NameDA)."
!endif

!ifdef MUI_INSTFILESPAGE
  ${LangFileString} MUI_TEXT_INSTALLING_TITLE "Instalando"
  ${LangFileString} MUI_TEXT_INSTALLING_SUBTITLE "Por favor, aguarde enquanto o $(^NameDA) est� sendo instalado."
  ${LangFileString} MUI_TEXT_FINISH_TITLE "Instala��o completada"
  ${LangFileString} MUI_TEXT_FINISH_SUBTITLE "A instala��o foi conclu�da com sucesso."
  ${LangFileString} MUI_TEXT_ABORT_TITLE "Instala��o abortada"
  ${LangFileString} MUI_TEXT_ABORT_SUBTITLE "A instala��o n�o foi conclu�da com sucesso."
!endif

!ifdef MUI_UNINSTFILESPAGE
  ${LangFileString} MUI_UNTEXT_UNINSTALLING_TITLE "Desinstalando"
  ${LangFileString} MUI_UNTEXT_UNINSTALLING_SUBTITLE "Por favor, aguarde enquanto o $(^NameDA) est� sendo desinstalado."
  ${LangFileString} MUI_UNTEXT_FINISH_TITLE "Desinstala��o completada"
  ${LangFileString} MUI_UNTEXT_FINISH_SUBTITLE "A desinstala��o foi conclu�da com sucesso."
  ${LangFileString} MUI_UNTEXT_ABORT_TITLE "Desinstala��o abortada"
  ${LangFileString} MUI_UNTEXT_ABORT_SUBTITLE "A desinstala��o n�o foi conclu�da com sucesso"
!endif

!ifdef MUI_FINISHPAGE
  ${LangFileString} MUI_TEXT_FINISH_INFO_TITLE "Concluindo o assistente de instala��o do $(^NameDA)"
  ${LangFileString} MUI_TEXT_FINISH_INFO_TEXT "$(^NameDA) foi instalado no seu computador.$\r$\n$\r$\nClique em Terminar para fechar este assistente."
  ${LangFileString} MUI_TEXT_FINISH_INFO_REBOOT "Seu computador deve ser reiniciado para concluir a instala��o do $(^NameDA). Voc� deseja reiniciar agora?"
!endif

!ifdef MUI_UNFINISHPAGE
  ${LangFileString} MUI_UNTEXT_FINISH_INFO_TITLE "Concluindo o assistente de desinstala��o do $(^NameDA)"
  ${LangFileString} MUI_UNTEXT_FINISH_INFO_TEXT "$(^NameDA) foi desinstalado do seu computador.$\r$\n$\r$\nClique em Terminar para fechar este assistente."
  ${LangFileString} MUI_UNTEXT_FINISH_INFO_REBOOT "Seu computador deve ser reiniciado para completar a desinstala��o do $(^NameDA). Voc� deseja reiniciar agora?"
!endif

!ifdef MUI_FINISHPAGE | MUI_UNFINISHPAGE
  ${LangFileString} MUI_TEXT_FINISH_REBOOTNOW "Reiniciar agora"
  ${LangFileString} MUI_TEXT_FINISH_REBOOTLATER "Eu desejo reinici�-lo manualmente depois"
  ${LangFileString} MUI_TEXT_FINISH_RUN "&Executar $(^NameDA)"
  ${LangFileString} MUI_TEXT_FINISH_SHOWREADME "&Mostrar o Leia-me"
  ${LangFileString} MUI_BUTTONTEXT_FINISH "&Terminar"
!endif

!ifdef MUI_STARTMENUPAGE
  ${LangFileString} MUI_TEXT_STARTMENU_TITLE "Escolher uma pasta do Menu Iniciar"
  ${LangFileString} MUI_TEXT_STARTMENU_SUBTITLE "Escolha uma pasta do Menu Iniciar para os atalhos do programa."
  ${LangFileString} MUI_INNERTEXT_STARTMENU_TOP "Selecione uma pasta do Menu Iniciar em que deseja criar os atalhos do programa. Voc� pode tamb�m digitar um nome para criar uma nova pasta."
  ${LangFileString} MUI_INNERTEXT_STARTMENU_CHECKBOX "N�o criar atalhos"
!endif

!ifdef MUI_UNCONFIRMPAGE
  ${LangFileString} MUI_UNTEXT_CONFIRM_TITLE "Desinstalar o $(^NameDA)"
  ${LangFileString} MUI_UNTEXT_CONFIRM_SUBTITLE "Remova o $(^NameDA) do seu computador."
!endif

!ifdef MUI_ABORTWARNING
  ${LangFileString} MUI_TEXT_ABORTWARNING "Voc� tem certeza que deseja finalizar a instala��o do $(^Name)?"
!endif

!ifdef MUI_UNABORTWARNING
  ${LangFileString} MUI_UNTEXT_ABORTWARNING "Voc� tem certeza que deseja finalizar a desinstala��o do $(^Name)?"
!endif

!ifdef MULTIUSER_INSTALLMODEPAGE
  ${LangFileString} MULTIUSER_TEXT_INSTALLMODE_TITLE "Escolher usu�rios"
  ${LangFileString} MULTIUSER_TEXT_INSTALLMODE_SUBTITLE "Escolha para quais usu�rios deseja instalar o $(^NameDA)."
  ${LangFileString} MULTIUSER_INNERTEXT_INSTALLMODE_TOP "Selecione se deseja instalar o $(^NameDA) apenas para si mesmo ou para todos os usu�rios deste computador. $(^ClickNext)"
  ${LangFileString} MULTIUSER_INNERTEXT_INSTALLMODE_ALLUSERS "Instalar para todos que usam este computador"
  ${LangFileString} MULTIUSER_INNERTEXT_INSTALLMODE_CURRENTUSER "Instalar apenas para mim"
!endif