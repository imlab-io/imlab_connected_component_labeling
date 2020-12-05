---
layout: post
title: Bağlantılı Bileşen Etiketleme
slug: connected-component-labeling
author: Bahri ABACI
categories:
- Görüntü İşleme
- Hızlı Algoritmalar
references: "A Simple and Efficient Connected Components Labeling Algorithm"
thumbnail: /assets/post_resources/connected_component_labeling/thumbnail.png
---

Bağlantılı bileşen etiketleme siyah-beyaz görüntüler üzerine uygulanan ve birbiri ile komşu olan piksellerin bir grup içerisinde toplamaya yarayan bir işlemdir. Bu gruplama sonucunda, resim üzerindeki her bir grup bir nesneyi temsil edecek şekilde numaralandırılır. Daha sonra da istenen grup numaralı nesne üzerinde kolaylıkla işlem yapılabilir. Bağlantılı bileşen etiketleme algoritması 4-komşuluk ve 8-komşuluk olarak ikiye ayrılır. Burada 8 komşuluk seçilirse çapraz piksellerin de komşu olduğu kabul edilmiş olur. Uygulamalarda genellikle 8-komşuluk tercih edildiğinden bu yazımızda da 8-komşuluk algoritması üzerinde duruldu ve çapraz pikseller de komşu olarak işlemlere dahil edildi.  

<!--more-->
  
### Algoritmanın Detayları
  
Bağlantılı bileşen etiketleme için pek çok algoritma önerilse de yazımızda bahsedeceğimiz yöntem literatürde sıklıkla kullanılan ve anlaması diğer yöntemlere göre nispeten kolay olan metot Two-Pass (Çift Geçiş) yöntemidir. Bu yöntemde ilk geçişte tüm pikseller tek tek gezilerek eğer gözek siyaha eşit değilse şu algoritma işletilir.  
  
-   Pikselin Tüm komşularına (8 adet) bak
-   Tüm komşular siyah veya beyaz ise bu yeni bir pikseldir,  piksele yeni bir etiket ata, diğer piksele geç
-   Komşulardan en az biri etiketli (siyah veya beyaz değil) ise piksele  etiketli komşu / ların en küçük etiketlisini ata, diğer etiketlerin aynı olduğunu kaydet

Algoritmayı detaylı olarak gösteren animasyon aşağıda görülmektedir. Görüldüğü üzere resim piksel piksel gezilerek yukarıdaki algoritmaya göre etiketlenmektedir. Komşu pikseller içerisinde farklı etiketlere rastlandığında ise küçük etiket piksele atanmakta ve büyük etiketin aslında küçük etiket ile aynı olduğu bilgisi saklanmaktadır. Yanda ise ilk geçiş sonrası elde edilen görüntü bulunmaktadır.  

| Bağlantılı Bileşen Etiketleme Animasyonu   |  Bağlantılı Bileşen Etiketleme Sonucu |
|:----------:|:-----------------:|
![Bağlantılı bileşen Etiketleme Animasyonu][ccl_animation] | ![Sabit Eşik ile Eşikleme][ccl_result] 
  
Görüldüğü gibi ilk geçiş sonrası oluşan şekilde 1 ve 2 numaralı nesneler farklı nesnelermiş gibi görünseler de aslında bağlantılı (aynı) nesnelerdir. İkinci geçiş bunu önlemek için vardır. İkini geçişte piksellere içerisindeki değerin gösterdiği değer yazılır. Mesela örneğimizde ikinci geçiş sonrası 2 etiketi görülen piksellere tabloya bakılarak (<span style="color: #0b5394;">2-&gt;1</span>) 1 etiketi yazılır. Böylece tüm bağımlı nesneler aynı etiket ile etiketlenmiş olur.  
  
Böyle bir etiketleme işlemi sonucunda her ayrık nesne farklı bir etikete sahip olacaktır ancak bu etiketleri bilmeden nesneleri ayırmak mümkün olmayacağından, işlem sonrasında kullanılan etiketler bir etiket dizisi şeklinde { `etiket[]={1,3,4,5}` gibi (__dikkat 2 yerine 1 yazıldığından 2 döndürülmüyor__} dışarıya döndürülmelidir.  
  
Aşağıda yazılan fonksiyonda bunun yerine sıralı etiket oluşturmayı seçilmiştir. Bu teknikle etiket dizisi {1,2,3,4} (yani ayrık nesne sayısına kadar sıralı) şekilde elde edildiğinden dizi döndürmek yerine etiketlerin en büyüğünü yani ayrık nesne sayısını döndürmek yeterli olacaktır. Bu tekniği uygulamak için sırası gelen pikselin değerinin başka bir piksele bağlı olup olmadığına bakılır. Eğer bağlı değilse bunun yeni bir nesne olduğu anlaşılır ve nesne sayısı 1 artırılarak, piksele bu sayı yazılır ve eski değerin yeni değeri işaret ettiği yeni bir vektör üzerinde işaret edilir. Bağlı ise de bağlı olduğu eleman bulunur ve bağlı olduğu elemanın gösterdiği sayı yazılır.

### Kodlama Zorlukları

-   Yazılan kodda bir karışıklık yaratmaması için beyaz pikseller 255->1 şeklinde dönüştürülmüştür ve etiketler 2 den başlayarak verilmiştir. (Etiket sayısı 255 olursa bu etiket mi ? Beyaz piksel mi? karışabilir)
-   Resim üzerinde bir kanalda 255 renk (etiket) saklanabileceğinden etiket sayısı 255 i geçtiğinde yeni etiketler eskisini silecekti. Be nedenle etiket imgesi uint32_t tipinde oluşturularak 32 bit etiket numaraları dahi saklanabilir yapılmıştır.
-   Hesaplamada hız kazanmak için 8 komşudan daha etiketlenmemiş olduğu kesin olan öndeki 4 komşuya (sağ, sol alt, alt ve sağ alt) bakılmamıştır.

Verilen bir ikili resimde 8-komşuluklu bağlantılı bileşenleri etiketleme fonksiyonu şu şekilde yazılmıştır.  
  
```c
// connected component labeling algorithm as presented in cescript blog (buggy)
void connected_component_label(matrix_t *input, matrix_t *output, uint32_t *numCC)
{
    // set the number of connected componenets to zero
    numCC[0] = 0;

    int cond = channels(input) == 1 && is_8u(input) && is_32u(output);
    
    // if the condition is not met, return
    if(!cond)
    {
        printf("input must be uint8 array with single channel and output must be uint32 array!\n");
        return;
    }

    // get the pointer to the data
    uint8_t *in_data = data(uint8_t, input);
    uint32_t *out_data = data(uint32_t, output);

    // resize the output label matrix
    matrix_resize(output, rows(input), cols(input), 1);

    // allocate the list array
    uint32_t *list = calloc(volume(input) / 4, sizeof(uint32_t)); //en fazla kaç ayrık nesne olabilir

    // label number for the next component, 0 background, 1 foreground
    uint32_t current_label = 2;

    // neighbours of the current pixel (r,c)
    uint32_t neigh[4];

    // make sure that the input is in 0-1 range
    uint32_t idx = 0;
    for(idx = 0; idx < volume(input); idx++)
    {
        out_data[idx] = in_data[idx] > 0 ? 1 : 0;
    }
    
    uint32_t r = 0, c = 0;
    for (int r = 1; r < rows(input) - 1; r++)
    {
        for (int c = 1; c < cols(input) - 1; c++)
        {
            // if the current pixel is foreground
            if (at(uint32_t, output, r,c) != 0)
            {
                // get the four neighbours of the current pixel
                neigh[0] = at(uint32_t, output, r, c - 1);
                neigh[1] = at(uint32_t, output, r - 1, c);
                neigh[2] = at(uint32_t, output, r - 1, c - 1);
                neigh[3] = at(uint32_t, output, r - 1, c + 1);

                // check all the neighbours, if all of them are unlabeled
                if (neigh[0] == 0 && neigh[1] == 0 && neigh[2] == 0 && neigh[3] == 0)
                {
                    // set the current label of the output to the next label
                    at(uint32_t, output, r, c) = current_label;

                    // update the connection list
                    list[current_label] = current_label;

                    // go to the next label
                    current_label++;
                }
                // if one of the neighbour is labeled, find the root of the label
                else
                {
                    // sort the labels of the neighbours
                    sort_neighbours(neigh);

                    // compare the labels of the neighbours 
                    if (neigh[0] != 0 && neigh[1] != neigh[0])
                    {
                        list[neigh[1]] = find_root(list, neigh[0]);
                    }
                    if (neigh[1] != 0 && neigh[2] != neigh[1])
                    {
                        list[neigh[2]] = find_root(list, neigh[1]);
                    }
                    if (neigh[2] != 0 && neigh[3] != neigh[2])
                    {
                        list[neigh[3]] = find_root(list, neigh[2]);
                    }

                    // starting from the smallest label, pick the first one different from the background
                    uint32_t l = 0;
                    for (int l = 0; l < 4; l++)
                    {
                        // pick the first one different from the background
                        if (neigh[l] > 0)
                        {
                            at(uint32_t, output, r, c) = neigh[l];
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Create Consecutive Label
    uint32_t *final_list = calloc(current_label, sizeof(uint32_t)); 

    // go over all the labels one more time and make them consecutive
    uint32_t l = 0;
    for (l = 2; l < current_label; l++)
    {
        if (l == find_root(list, l))
        {
            // new component found
            numCC[0]++; 
            final_list[l] = numCC[0];
        }
    }

    // re assign the labels that are assigned previously
    for (idx = 0; idx < volume(output); idx++)
    {
        out_data[idx] = final_list[find_root(list, out_data[idx])];
    }

    // clean unused arrays
    free(list);
    free(final_list);
}
```

Burada `find_root` fonksiyonu verilen bir bileşen numarasının gerçekte kime bağlı olduğunu bulmak için yazılmıştır. Yukarıdaki örneği göz önünde bulunduracak olursak find_root(list,1), 1 değerini döndürürken find_root(list,2) de 1 değerini döndürecektir. Özellikle çoklu bağlantının söz konusu olduğu durumlarda bu fonksiyon tüm bağlantıları inceleyerek en küçük bağlantı numarasını döndürecektir. Örnek olarak etiketleme devam ederken 5 numaralı bileşenin de 2 numaralı bileşene bağlı olduğu görülebilir. Bu durumda find_root(list, 5) ifadesi 1 değerini döndürecektir. Fonksiyonun içeriği aşağıda verilmiştir.  
  
```c
// find the minimum label assigned to this value by tracing the connected labels
uint32_t find_root(uint32_t *list, uint32_t value)
{
    while (value != list[value])
    {
        value = list[value];
    }
    return value;
}
```
  
Örnek olarak [bir önceki yazıda](http://cescript.blogspot.com/2012/09/c-ile-gabor-filtre-uygulamasi.html) elde ettiğimiz plaka aday bölgeleri etiketleyebiliriz.
  
```c
// read image
matrix_t *img   = imread("..//data//araba.bmp");

// allocate gray, black-white and labeled image
matrix_t *grayimg = matrix_create(uint8_t, rows(img), cols(img), 1);
matrix_t *bwimg = matrix_create(uint8_t, rows(img), cols(img), 1);
matrix_t *label = matrix_create(uint32_t, rows(img), cols(img), 1);

// convert input to grayscale
rgb2gray(img, grayimg);

// binarize the grayscale image
imthreshold(grayimg, 128, bwimg);

// find connected components
uint32_t conn = 0;
//bwlabel(bwimg, label, &conn);
connected_component_label(bwimg, label, &conn);

// print the number of labels
printf("Label: %d\n", conn);

// assign different color for eack connected component
label2rgb(label, conn, img);

// test output
imwrite(img, "..//data//araba_plaka.bmp");

```

Kod içerisinde yer alan `label2rgb` içerisinde `uint32_t` biçiminde hesaplanan bağlantılı bileşen numaraları farklı sayılarla çarpılarak sebebi sıralı renkler yerine farklı renkler elde etmek için kullanılmaktadır. Verilen görüntü üzerinde toplam 14 bağlantılı bileşen tespit edilmiştir ve sonuçlar aşağıdaki tabloda görselleştirilmiştir.

| Bağlantılı Bileşen Etiketleme Örnek   |  Bağlantılı Bileşen Etiketleme Sonucu |
|:----------:|:-----------------:|
![Bağlantılı bileşen Etiketleme Örnek][car_license_plate] | ![Sabit Eşik ile Eşikleme][car_license_plate_ccl] 
  

Son olarak sınırları zorlayan bir resim ile kodumuzu test edelim. Resim 1024x1024 büyüklükte ve çok sayıda bağlantılı nesne içeren bir resim. Bu resim ile yapılan testlerde toplam 2173 ayrık nesne bulunmuştur (resim üzerinde farklı gruplar aynı etikete sahip gibi görünse de her renk birbirinden farklıdır).

| Bağlantılı Bileşen Etiketleme Örnek   |  Bağlantılı Bileşen Etiketleme Sonucu |
|:----------:|:-----------------:|
![Bağlantılı bileşen Etiketleme Örnek][multiple] | ![Sabit Eşik ile Eşikleme][multiple_ccl] 

Yazımızda verilen örnekler ve uygulama koduna yazının [GitHub Sayfasından](https://github.com/cescript/imlab_connected_component_labeling) erişebilirsiniz.

**<span style="color: red;">NOT: </span> Kodda var olan bir hatadan dolayı [GitHub Sayfasında](https://github.com/cescript/imlab_connected_component_labeling) da verilen karmaşık örneklerde algoritma düzgüün çalışmamaktadır. IMLAB görüntü işleme kütüphanesinde burada anlatılan yönteme göre çok daha karışık ve hızlı bir algoritma `bwlabel` fonksiyonu ile gerçeklenmiştir. Eğer bu işleme projenizde ihtiyaç duyuyor iseniz yazımızda verilen `connected_component_label` ile aynı girdi ve çıktılara sahip `bwlabel` fonksiyonunu kullanmanız tavsiye edilir.**

**Referanslar**

* Di Stefano, Luigi, and Andrea Bulgarelli. "A simple and efficient connected components labeling algorithm." Proceedings 10th International Conference on Image Analysis and Processing. IEEE, 1999.

* He, Lifeng, et al. "Fast connected-component labeling." Pattern recognition 42.9 (2009): 1977-1987.

[RESOURCES]: # (List of the resources used by the blog post)
[ccl_animation]: /assets/post_resources/connected_component_labeling/ccl_animation.gif
[ccl_result]: /assets/post_resources/connected_component_labeling/ccl_result.png
[car_license_plate]: /assets/post_resources/connected_component_labeling/car_license_plate.png
[car_license_plate_ccl]: /assets/post_resources/connected_component_labeling/car_license_plate_ccl.png
[multiple]: /assets/post_resources/connected_component_labeling/multiple.png
[multiple_ccl]: /assets/post_resources/connected_component_labeling/multiple_ccl.png