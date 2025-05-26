# TP_Proiect
TP_Proiect


Joc Farming Simulator
1. Prezentare generală
TP_Proiect este un joc de tip farming inspirat de Hay Day, dezvoltat în limbajul C și compilat cu Emscripten pentru rulare în browser. Jucătorul poate cultiva grâu și roșii, produce pâine și vinde produse la un mini shop. Există și o jucărie tip casino slot ca element de interacțiune opțională. Jocul este construit cu SDL2 și rulează în context WebAssembly.
 
2. Mini Shop – Economie în joc
Jucătorul are posibilitatea să vândă produse din fermă: pâine, grâu și roșii. Fiecare produs are un preț fix, iar tranzacțiile sunt gestionate în funcție de cantitatea stocată.
 
3. Casino Slot Machine – Funcție de randomizare
Casino-ul oferă o experiență de slot machine. Jucătorul alege o sumă de pariat, apasă SPIN, iar 3 simboluri sunt afișate aleator. Dacă toate simbolurile coincid, jucătorul câștigă. Funcția folosește `rand()` pentru generarea simbolurilor și le redă cu SDL.
 
4. Culturi și creșterea roșiilor
Roșiile pot fi plantate pe parcele de teren. După o durată prestabilită, devin coapte și pot fi recoltate. Acest ciclu este gestionat prin variabile logice (`tomato_e_copt`) și evenimente SDL de click.
 
5. Funcția main_loop – Coordonare logică joc
Funcția `main_loop()` gestionează toate evenimentele și stările grafice. Este responsabilă pentru detectarea evenimentelor de click, afișarea interfețelor (modal shop, barn, slot machine), și pentru inițializarea texturilor la rulare. Fiecare element de joc (ferma, casino, fabrică) are propriul său buton.
Cod parțial:
if (!show_modal_shop) {
    if (handleButtonWithCondition(renderer, NULL, buttonTextureBarn, ...)) {
        show_modal_shop = true;
    }
}
if (show_modal_shop) {
    render_modal_shop(renderer, &show_modal_shop, title_font);
}

 
