
# Id: $Author: merzky $ on $Date: 2002/09/26 13:05:38 $ ($Revision: 1.4 $) 

#####################################################################
# project name

BASE        = nesc-saga-intro

#####################################################################
# executeables

TEX         = latex
PDFTEX      = pdflatex
SPELL       = ispell # -d deutsch
PSBOOK      = psbook
DVIPS       = dvips
BIBTEX      = bibtex
MAKEINDEX   = makeindex
MAKE_IND    = $(MAKEINDEX) -s gind.ist 
MAKE_GLS    = $(MAKEINDEX) -s gglo.ist 
MAKE_GLS    = ./glo.pl
PS2SORT     = ps2sort
FIG2DEV	    = fig2dev 
PS2EPSI	    = ps2epsi
PSTOPDF     = ps2pdf
EPSTOPDF    = epstopdf
LATEX2HTML  = latex2html
XDVI        = xdvi -geometry 843x1024+0+0 -s 3
XDVI_LOCK   = .xdvi_lock
ACRO        = open # acroread
ACRONEW     = echo "" # acronew
ACRO_LOCK   = .acro_lock
RM          = rm -f
TOUCH       = touch
LPR         = lpr

#####################################################################
# files

TEXDIR      = .
BIBDIR      = .
PICDIR      = Pics
FIGDIR      = Pics

TEXFILES    = $(wildcard $(TEXDIR)/*.tex)
BIBFILES    = $(wildcard $(BIBDIR)/$(BASE).bib)
PICS        = $(wildcard $(PICDIR)/*.{eps,ps,jpeg,jpg,gif,png,pdf})
FIGS        = $(wildcard $(PICDIR)/*.fig)

TEX_FIGS    = $(addsuffix .eps,$(basename $(FIGS)))
PDF_FIGS    = $(addsuffix .pdf,$(basename $(FIGS)))

#####################################################################
# special targets

.SECONDARY:   $(TEX_FIGS) $(PDF_FIGS)

.SUFFIXES:    .tex .dvi .eps .ps  .ps2 .ps4 .pdf \
              .bib .bbl .fig .idx .ind .glo .gls
              
.PHONY:       clean distclean figclean \
              show   s                 \
	      again  a                 \
              print  p                 \
              print2 p2                \
              spell                    \
              touch  t                 \
              edit   e                 \
              Edit   E                 \
              fine                     \
              bib bbl ind gls          \
              ps ps2 pdf dvi
              
#####################################################################
# implicit rules
              
%.dvi:	      %.tex
%.dvi:        %.tex $(TEXFILES) $(PICS) $(TEX_FIGS) \
              $(addsuffix .bbl,$(basename $(BIBFILES)))
	      @echo FIGS = $(TEX_FIGS)
	      $(TEX) $<
              
#%.pdf:       %.tex
#%.pdf:       %.tex $(TEXFILES) $(PICS) $(PDF_FIGS)
#	      $(PDFTEX) $<
              
%.pdf:        %.ps
%.pdf:        %.ps 
	      $(PSTOPDF) $<
              
%.ps:         %.dvi
%.ps:         %.dvi
	      $(DVIPS) $< -o $@
              
%.ps2:        %.ps
%.ps2:        %.ps
#	      $(PS2SORT) $< $@
	      $(PSBOOK)  $< $@
              
%.bbl:        %.bib
%.bbl:        %.bib
	      $(if $(wildcard $(BASE).aux), $(BIBTEX) $* )
              
%.gls:        %.glo
%.gls:        %.glo 
	      $(MAKE_GLS) -o $@ $<
              
%.ind:        %.idx
%.ind:        %.idx
	      $(MAKE_IND) -o $@ $<
              
%.eps:        %.fig
%.eps:        %.fig
	      $(FIG2DEV) -Leps $< $@
              
%.ps:         %.fig
%.ps:         %.fig
	      $(FIG2DEV) -Lps $< $@
              
%.eps:        %.ps
%.eps:        %.ps
	      $(PS2EPSI) $< $@
              
%.pdf:        %.eps
%.pdf:        %.eps
	      $(EPSTOPDF) $<
              
#####################################################################
# short targets

s:            slides
a:            again
t:            touch
e:            edit
E:            Edit
p:            print
p2:           print2

ps:           $(BASE).ps
ps2:          $(BASE).ps2
pdf:          $(BASE).pdf
dvi:          $(BASE).dvi
tex:          $(BASE).tex
ind:          $(BASE).ind
gls:          $(BASE).gls
bbl:          $(BASE).bbl
aux:          $(BASE).aux
              
#####################################################################
# targets

spell:        
	      $(SPELL) $(TEXFILES)
              
print:        $(BASE).ps
	      $(LPR) $(BASE).ps
              
print2:       $(BASE).ps2
	      $(LPR) $(BASE).ps2; 
              
touch:        
	      $(TOUCH) $(TEXFILES) $(BIBFILES)
              
edit:         
	      $(EDITOR) $(TEXFILES) $(BIBFILES) &
              
Edit:         
	      $(EDITOR) $(TEXFILES) $(BIBFILES) \
	      $(wildcard GNUmakefile* makefile* Makefile*) &
              
#--------------------------------------------------
# if an xdvi starts, it creates an XDVI_LOCK file.
# if this exists, it does not start a new process.
# if xdvi finishes, it removes the XDVI_LOCK file.
# 
show:         $(BASE).dvi
	      @if [ ! -r $(XDVI_LOCK) ]; then \
	        ($(TOUCH) $(XDVI_LOCK) ;      \
	         $(XDVI)  $(BASE).dvi  ;      \
        	 $(RM)    $(XDVI_LOCK) ;      \
        	)&                            \
	      fi
              
slides:       $(BASE).pdf
	      @if [ ! -r $(ACRO_LOCK) ]; then \
	        ($(TOUCH) $(ACRO_LOCK) ;      \
	         $(ACRO)  $(BASE).pdf  ;      \
	         $(RM)    $(ACRO_LOCK) ;      \
	        )&                            \
        else                            \
          $(ACRONEW);                   \
	      fi
              
again:
	      $(MAKE) -W $(BASE).tex $(filter-out again, $(MAKECMDGOALS))
	      
fine:         
	      $(MAKE) distclean
	      $(MAKE) touch
	      $(MAKE) pdf
	      $(MAKE) dvi
	      $(MAKE) ind
	      $(MAKE) gls
	      $(MAKE) again dvi
	      $(MAKE) again dvi
	      $(MAKE) ps
	      $(MAKE) ps2
	      $(MAKE) clean
              
clean:        
	      $(MAKE) figclean
	      $(RM) \
	        $(BASE).{aux,blg,bbl,log,toc,err,idx,bak,glo,gls,ilg,ind,lof,lot,out} \
                .*.sw[po] *~
              
distclean:    
	      $(MAKE) clean
	      $(RM) $(BASE).{bak,dvi,ps,ps2,pdf,tex.bak}
	      $(RM) $(XDVI_LOCK) $(ACRO_LOCK) makelog $(FIGDIR)/*.bak
              
figclean:     
	      $(RM) $(TEX_FIGS) $(PDF_FIGS)

print4:
	      cp talk.pdf print.pdf
				pdfps print.pdf
				rm print.pdf
				psnup -4 print.ps > print.ps4

.PHONY: install
install: pdf
	cp nesc-saga-intro.pdf ~/links/saga/projects/tutorial/NeSC-2009/saga_nesc09_api.pdf
              
#####################################################################
# dependencies

$(BASE).aux:  $(BASE).dvi
$(BASE).log:  $(BASE).dvi
$(BASE).toc:  $(BASE).dvi
$(BASE).glo:  $(BASE).dvi
$(BASE).idx:  $(BASE).dvi
              
