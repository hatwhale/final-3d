/*
Контрольное домашнее задание № 1.
Касьянов Илья Алексеевич, БМТ142
18.03.18
Visual Studio 2010
Windows 8.1 x64
Intel(R) Core(TM) i5-4200M CPU @ 2.50GHz 2.49 GHz
4ГБ оперативной
DirectX 11

В этом проекте реализованы:
- загрузка 3D объектов и параметров их текстурирования из файлов,
процедурное построение и текстурирование моделей 3D объектов:
реализована генерация платоновых тел, сферы и ее производных, тора и его производных;
для платоновых тел одна и та же текстура накладывается на каждую грань,
для сферы, тора и их производных текстура накладывается на весь объект;
(соответственно получению данной модели из цилиндра, на который и накладывается текстура,
подробнее - в model.h);

- аффинные преобразования объектов и пространства:
реализованы масштабирование, сдвиги вдоль и вращения вокруг осей посредством мыши (инструкция по нажатию кнопки info),
реализовано движение каждого объекта в отдельности;
реализованы режимы волчка и карусели;
(подробнее - algebra_tool.h, model.h)
режим волчка реализован посредством сдвига в начало координат, вращения и сдвига обратно в цикле;
режим карусели реализован посредством вращения в цикле;

- получение параллельной и перспективной проекций сцены:
реализованы два вида проекции - параллельная и одноточечная перспективная, между которыми можно переключаться;
(подробнее - algebra_tool.h, render_tool.h);

- вывод каркасных моделей и сплошных объектов:
реализовано переключение между двумя режимами отображения;
(подробнее - render_tool.h)

- отсечение ближней и дальней плоскостями отсечения:
реализовано при помощи буфера глубины;
(подробнее - render_tool.h)

- удаление невидимых ребер каркасных моделей и невидимых граней:
реализован как алгоритм Робертса, так и удаление посредством буфера глубины;
для пересекающихся каркасных моделей новые ребра не строятся;
(подробнее - render_tool.h)

- методы закраски объектов - плоской, Гуро и Фонга:
(подробнее - render_tool.h)

- включение / выключение точечного источника света:
включение источника света не влияет на каркасную модель;
при выключенном источники света каждый объект равномерно закрашивается своим цветом;
(подробнее - render_tool.h)

- движение точечного источника света:
реализованы масштабирование, сдвиги вдоль и вращения вокруг осей  координат источника света
(аналогично движению объектов, подробнее в info);
релизовано переключение между режимами движения объектов и источника света;

- включение / выключение режима построения теней;

- регулирование свойств материалов выбранных объектов,
а также привязка различных текстур;
(подробнее - data_structures.h)

- вывод на экран и сохранение буферов кадра и глубины:
буфер глубины записывается при включенном тесте глубины;
буфер псевдо-глубины записывается при включенном теневом тесте;
при включенном режиме движения источника света и теневом тесте отображаются:
вместо буфера глубины - буфер псевдо-глубины, shadow map,
вместо буфера кадра - буфер псевдо-кадра с проверками на глубину,
(прочие настройки по умолчанию, подробнее в data_structures.h)
с камерой в источнике освещения и направлением в центр сцены (0, 0, 0);
(подробнее - render_tool.h)

- сохранение матрицы вершин и текстурных вершин, списка граней и текстурных граней последней добавленной модели;

В matrix.h и matrix.cpp  реализован класс Matrix, реализующий матрицы размерности n,
в vector.h и vector.cpp реализован класс Vector, реализующий вектор, наследованный от List<double>,
в algebra_tool.h и algebra_tool.cpp реализованы операции с классами Matrix и Vector;
в model.h и model.cpp реализовано описание полигонального объекта и афинные преобразования над ним;
в geom_tool.h и geom_tool.cpp реализованы построения массива пикселей для прямых и полигонов;
в render_tool.h и render_tool.cpp реализованы применение проективного преобразование к моделям и их отображение;
в data_structures.h перечислены используемые структуры,

более подробно - в соответствующих заголовочных файлах.
*/

#pragma once

#include "render_tool.h"

namespace rendering {
	#include "model.h"
	#include "vector.h"
	#include "data_structures.h"

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;

	public ref class RenderForm : public System::Windows::Forms::Form
	{
	private:
		EventData^ event_data;
		RenderData^ render_data;
		LightData^ light_data;

		RenderTool^ render_tool;
		List<Model^> ^models;

	private: System::Windows::Forms::Button^  rotateButton;
	private: System::Windows::Forms::Button^  translateButton;
	private: System::Windows::Forms::Button^  projectButton;
	private: System::Windows::Forms::Button^  infoButton;
	private: System::Windows::Forms::OpenFileDialog^  loadFileDialog;
	private: System::Windows::Forms::Timer^  pegTopTimer;
	private: System::Windows::Forms::Timer^  carouselTimer;
	private: System::Windows::Forms::Button^  popButton;
	private: System::Windows::Forms::Button^  scaleButton;
	private: System::Windows::Forms::CheckBox^  transparencyСheckBox;
	private: System::Windows::Forms::CheckBox^  pegTopCheckBox;
	private: System::Windows::Forms::CheckBox^  carouselCheckBox;
	private: System::Windows::Forms::ComboBox^  modelComboBox;
	private: System::Windows::Forms::CheckBox^  fillCheckBox;
	private: System::Windows::Forms::Label^  lightLabel;
	private: System::Windows::Forms::TrackBar^  lightAmbTrackBar;
	private: System::Windows::Forms::TrackBar^  lightDiffTrackBar;
	private: System::Windows::Forms::Label^  lightDiffLabel;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  matSpecLabel;
	private: System::Windows::Forms::TrackBar^  matSpecTrackBar;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  matDiffLabel;
	private: System::Windows::Forms::TrackBar^  matDiffTrackBar;
	private: System::Windows::Forms::Label^  sheenLabel;
	private: System::Windows::Forms::TrackBar^  matSheenTrackBar;
	private: System::Windows::Forms::PictureBox^  pictureBox;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::Button^  imageSaveButton;
	private: System::Windows::Forms::CheckBox^  depthCheckBox;
	private: System::Windows::Forms::Button^  modelSaveButton;
	private: System::Windows::Forms::ComboBox^  shadingComboBox;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  activateLightButton;



	private: System::Windows::Forms::Button^  renderModeButton;
	private: System::Windows::Forms::Button^  moveLightButton;
	private: System::Windows::Forms::ComboBox^  texComboBox;
	private: System::Windows::Forms::CheckBox^  shadowCheckBox;



	public: 
		Bitmap ^z_buffer;
		event System::EventHandler^ RenderEvent;
		System::String^ data_path;
		RenderForm(void)
		{
			InitializeComponent();
			this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &RenderForm::MouseWheelEvent);
			this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &RenderForm::Render);
			this->projectButton->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->pegTopTimer->Tick += gcnew System::EventHandler(this, &RenderForm::Render);
			this->carouselTimer->Tick += gcnew System::EventHandler(this, &RenderForm::Render);
			this->popButton->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->transparencyСheckBox->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->pegTopCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->carouselCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->modelComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &RenderForm::Render);
			this->fillCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->lightAmbTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::Render);
			this->lightDiffTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::Render);
			this->matSpecTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::Render);
			this->matDiffTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::Render);
			this->matSheenTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::Render);
			this->depthCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->shadingComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &RenderForm::Render);
			this->activateLightButton->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->moveLightButton->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->renderModeButton->Click += gcnew System::EventHandler(this, &RenderForm::Render);
			this->texComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &RenderForm::Render);
			this->shadowCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::Render);

			event_data = gcnew EventData();
			light_data = gcnew LightData(gcnew Vector(0, 0, -500, 1));
			render_data = gcnew RenderData(gcnew Rectangle(0, 0, pictureBox->Width, pictureBox->Height),
										   gcnew Vector(0, 0, -500, 1), false);
			render_tool = gcnew RenderTool(pictureBox->Width, pictureBox->Height, render_data, light_data);
			pictureBox->Image = render_tool->render_bitmap;

			models = gcnew List<Model^>();
			
			/*
			this->Add(gcnew Model("tetrahedron", System::Math::Sqrt(3)));
			this->Add(gcnew Model("octahedron", 1));
			this->Add(gcnew Model("icosahedron", System::Math::Sqrt((5 - System::Math::Sqrt(5))/2)));
			this->Add(gcnew Model("cube", System::Math::Sqrt(3)));
			this->Add(gcnew Model("dodecahedron", System::Math::Sqrt(3)));
			
			for each(Model^ model in models) {
				model->scale(100);
				model->Transform(gcnew Vector(-30, -30, -10), "rotate");
			}
			*/
			RenderEvent += gcnew System::EventHandler(this, &RenderForm::Render);
			RenderEvent(this, gcnew System::EventArgs());

			data_path =  Application::ExecutablePath;
			data_path = data_path->Substring(0, data_path->LastIndexOf('\\') + 1) + "data";
			System::IO::Directory::CreateDirectory(data_path + "\\models");
			System::IO::Directory::CreateDirectory(data_path + "\\textures");
		}

	protected:
		~RenderForm()
		{
			if (components)
			{
				delete components;
			}
			delete[] models;
		}


#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->pictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->rotateButton = (gcnew System::Windows::Forms::Button());
			this->translateButton = (gcnew System::Windows::Forms::Button());
			this->projectButton = (gcnew System::Windows::Forms::Button());
			this->infoButton = (gcnew System::Windows::Forms::Button());
			this->loadFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->pegTopTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->carouselTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->popButton = (gcnew System::Windows::Forms::Button());
			this->scaleButton = (gcnew System::Windows::Forms::Button());
			this->transparencyСheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->pegTopCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->carouselCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->modelComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->fillCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->lightLabel = (gcnew System::Windows::Forms::Label());
			this->lightAmbTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->lightDiffTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->lightDiffLabel = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->matSpecLabel = (gcnew System::Windows::Forms::Label());
			this->matSpecTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->matDiffLabel = (gcnew System::Windows::Forms::Label());
			this->matDiffTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->sheenLabel = (gcnew System::Windows::Forms::Label());
			this->matSheenTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->imageSaveButton = (gcnew System::Windows::Forms::Button());
			this->depthCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->modelSaveButton = (gcnew System::Windows::Forms::Button());
			this->shadingComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->activateLightButton = (gcnew System::Windows::Forms::Button());
			this->renderModeButton = (gcnew System::Windows::Forms::Button());
			this->moveLightButton = (gcnew System::Windows::Forms::Button());
			this->texComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->shadowCheckBox = (gcnew System::Windows::Forms::CheckBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightAmbTrackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightDiffTrackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matSpecTrackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matDiffTrackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matSheenTrackBar))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox
			// 
			this->pictureBox->Location = System::Drawing::Point(12, 53);
			this->pictureBox->Name = L"pictureBox";
			this->pictureBox->Size = System::Drawing::Size(648, 533);
			this->pictureBox->TabIndex = 0;
			this->pictureBox->TabStop = false;
			this->pictureBox->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &RenderForm::MouseDownEvent);
			this->pictureBox->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &RenderForm::MouseMoveEvent);
			this->pictureBox->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &RenderForm::MouseUpEvent);
			// 
			// rotateButton
			// 
			this->rotateButton->Location = System::Drawing::Point(489, 11);
			this->rotateButton->Name = L"rotateButton";
			this->rotateButton->Size = System::Drawing::Size(55, 35);
			this->rotateButton->TabIndex = 1;
			this->rotateButton->Text = L"rotate";
			this->rotateButton->UseVisualStyleBackColor = true;
			this->rotateButton->Click += gcnew System::EventHandler(this, &RenderForm::RotateEvent);
			// 
			// translateButton
			// 
			this->translateButton->Location = System::Drawing::Point(367, 11);
			this->translateButton->Name = L"translateButton";
			this->translateButton->Size = System::Drawing::Size(55, 35);
			this->translateButton->TabIndex = 2;
			this->translateButton->Text = L"translate";
			this->translateButton->UseVisualStyleBackColor = true;
			this->translateButton->Click += gcnew System::EventHandler(this, &RenderForm::TranslateEvent);
			// 
			// projectButton
			// 
			this->projectButton->Location = System::Drawing::Point(590, 12);
			this->projectButton->Name = L"projectButton";
			this->projectButton->Size = System::Drawing::Size(70, 35);
			this->projectButton->TabIndex = 3;
			this->projectButton->Text = L"orthogonal\r\nprojection";
			this->projectButton->UseVisualStyleBackColor = true;
			this->projectButton->Click += gcnew System::EventHandler(this, &RenderForm::ProjectSwitch);
			// 
			// infoButton
			// 
			this->infoButton->Location = System::Drawing::Point(12, 11);
			this->infoButton->Name = L"infoButton";
			this->infoButton->Size = System::Drawing::Size(35, 35);
			this->infoButton->TabIndex = 6;
			this->infoButton->Text = L"info";
			this->infoButton->UseVisualStyleBackColor = true;
			this->infoButton->Click += gcnew System::EventHandler(this, &RenderForm::InfoMessage);
			// 
			// pegTopTimer
			// 
			this->pegTopTimer->Interval = 1;
			this->pegTopTimer->Tick += gcnew System::EventHandler(this, &RenderForm::PegTop);
			// 
			// carouselTimer
			// 
			this->carouselTimer->Interval = 1;
			this->carouselTimer->Tick += gcnew System::EventHandler(this, &RenderForm::Сarousel);
			// 
			// popButton
			// 
			this->popButton->Location = System::Drawing::Point(53, 11);
			this->popButton->Name = L"popButton";
			this->popButton->Size = System::Drawing::Size(35, 35);
			this->popButton->TabIndex = 9;
			this->popButton->Text = L"pop";
			this->popButton->UseVisualStyleBackColor = true;
			this->popButton->Click += gcnew System::EventHandler(this, &RenderForm::Remove);
			// 
			// scaleButton
			// 
			this->scaleButton->Location = System::Drawing::Point(428, 11);
			this->scaleButton->Name = L"scaleButton";
			this->scaleButton->Size = System::Drawing::Size(55, 35);
			this->scaleButton->TabIndex = 10;
			this->scaleButton->Text = L"scale";
			this->scaleButton->UseVisualStyleBackColor = true;
			this->scaleButton->Click += gcnew System::EventHandler(this, &RenderForm::ScaleEvent);
			// 
			// transparencyСheckBox
			// 
			this->transparencyСheckBox->AutoSize = true;
			this->transparencyСheckBox->Location = System::Drawing::Point(683, 31);
			this->transparencyСheckBox->Name = L"transparencyСheckBox";
			this->transparencyСheckBox->Size = System::Drawing::Size(98, 17);
			this->transparencyСheckBox->TabIndex = 11;
			this->transparencyСheckBox->Text = L"early depth test";
			this->transparencyСheckBox->UseVisualStyleBackColor = true;
			this->transparencyСheckBox->Click += gcnew System::EventHandler(this, &RenderForm::EarlyDepthTestSwitch);
			// 
			// pegTopCheckBox
			// 
			this->pegTopCheckBox->AutoSize = true;
			this->pegTopCheckBox->Location = System::Drawing::Point(259, 11);
			this->pegTopCheckBox->Name = L"pegTopCheckBox";
			this->pegTopCheckBox->Size = System::Drawing::Size(62, 17);
			this->pegTopCheckBox->TabIndex = 12;
			this->pegTopCheckBox->Text = L"peg-top";
			this->pegTopCheckBox->UseVisualStyleBackColor = true;
			this->pegTopCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::PegTopSwitch);
			// 
			// carouselCheckBox
			// 
			this->carouselCheckBox->AutoSize = true;
			this->carouselCheckBox->Location = System::Drawing::Point(259, 29);
			this->carouselCheckBox->Name = L"carouselCheckBox";
			this->carouselCheckBox->Size = System::Drawing::Size(66, 17);
			this->carouselCheckBox->TabIndex = 13;
			this->carouselCheckBox->Text = L"carousel";
			this->carouselCheckBox->UseVisualStyleBackColor = true;
			this->carouselCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::CarouselSwitch);
			// 
			// modelComboBox
			// 
			this->modelComboBox->FormattingEnabled = true;
			this->modelComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(20) {L".txt", L"cube", L"tetrahedron", L"octahedron", 
				L"icosahedron", L"dodecahedron", L"cylinder", L"sphere", L"kinder", L"surprise", L"pear", L"almost_thorus", L"moon", L"drop", 
				L"fruit", L"garlic", L"thorus", L"spiral", L"spring", L"conical_spring"});
			this->modelComboBox->Location = System::Drawing::Point(12, 52);
			this->modelComboBox->Name = L"modelComboBox";
			this->modelComboBox->Size = System::Drawing::Size(76, 21);
			this->modelComboBox->TabIndex = 14;
			this->modelComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &RenderForm::ModelLoad);
			// 
			// fillCheckBox
			// 
			this->fillCheckBox->AutoSize = true;
			this->fillCheckBox->Location = System::Drawing::Point(683, 13);
			this->fillCheckBox->Name = L"fillCheckBox";
			this->fillCheckBox->Size = System::Drawing::Size(61, 17);
			this->fillCheckBox->TabIndex = 15;
			this->fillCheckBox->Text = L"flood fill";
			this->fillCheckBox->UseVisualStyleBackColor = true;
			this->fillCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::FillSwitch);
			// 
			// lightLabel
			// 
			this->lightLabel->AutoSize = true;
			this->lightLabel->Location = System::Drawing::Point(682, 265);
			this->lightLabel->Name = L"lightLabel";
			this->lightLabel->Size = System::Drawing::Size(58, 13);
			this->lightLabel->TabIndex = 16;
			this->lightLabel->Text = L"light chars:";
			// 
			// lightAmbTrackBar
			// 
			this->lightAmbTrackBar->AutoSize = false;
			this->lightAmbTrackBar->Location = System::Drawing::Point(710, 286);
			this->lightAmbTrackBar->Maximum = 100;
			this->lightAmbTrackBar->Name = L"lightAmbTrackBar";
			this->lightAmbTrackBar->Size = System::Drawing::Size(64, 23);
			this->lightAmbTrackBar->TabIndex = 17;
			this->lightAmbTrackBar->Tag = L"";
			this->lightAmbTrackBar->Value = 10;
			this->lightAmbTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::LightAmbChange);
			// 
			// lightDiffTrackBar
			// 
			this->lightDiffTrackBar->AutoSize = false;
			this->lightDiffTrackBar->Location = System::Drawing::Point(710, 315);
			this->lightDiffTrackBar->Name = L"lightDiffTrackBar";
			this->lightDiffTrackBar->Size = System::Drawing::Size(64, 23);
			this->lightDiffTrackBar->TabIndex = 18;
			this->lightDiffTrackBar->Tag = L"";
			this->lightDiffTrackBar->Value = 10;
			this->lightDiffTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::LightDiffChange);
			// 
			// lightDiffLabel
			// 
			this->lightDiffLabel->AutoSize = true;
			this->lightDiffLabel->Location = System::Drawing::Point(666, 286);
			this->lightDiffLabel->Name = L"lightDiffLabel";
			this->lightDiffLabel->Size = System::Drawing::Size(44, 13);
			this->lightDiffLabel->TabIndex = 20;
			this->lightDiffLabel->Text = L"ambient";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(666, 315);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(38, 13);
			this->label1->TabIndex = 21;
			this->label1->Text = L"diffuse";
			// 
			// matSpecLabel
			// 
			this->matSpecLabel->AutoSize = true;
			this->matSpecLabel->Location = System::Drawing::Point(666, 409);
			this->matSpecLabel->Name = L"matSpecLabel";
			this->matSpecLabel->Size = System::Drawing::Size(47, 13);
			this->matSpecLabel->TabIndex = 22;
			this->matSpecLabel->Text = L"specular";
			// 
			// matSpecTrackBar
			// 
			this->matSpecTrackBar->AutoSize = false;
			this->matSpecTrackBar->Location = System::Drawing::Point(710, 409);
			this->matSpecTrackBar->Name = L"matSpecTrackBar";
			this->matSpecTrackBar->Size = System::Drawing::Size(64, 23);
			this->matSpecTrackBar->TabIndex = 19;
			this->matSpecTrackBar->Tag = L"";
			this->matSpecTrackBar->Value = 2;
			this->matSpecTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::MatSpecChange);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(682, 359);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(75, 13);
			this->label3->TabIndex = 23;
			this->label3->Text = L"material chars:";
			// 
			// matDiffLabel
			// 
			this->matDiffLabel->AutoSize = true;
			this->matDiffLabel->Location = System::Drawing::Point(666, 380);
			this->matDiffLabel->Name = L"matDiffLabel";
			this->matDiffLabel->Size = System::Drawing::Size(38, 13);
			this->matDiffLabel->TabIndex = 27;
			this->matDiffLabel->Text = L"diffuse";
			// 
			// matDiffTrackBar
			// 
			this->matDiffTrackBar->AutoSize = false;
			this->matDiffTrackBar->Location = System::Drawing::Point(710, 380);
			this->matDiffTrackBar->Name = L"matDiffTrackBar";
			this->matDiffTrackBar->Size = System::Drawing::Size(64, 23);
			this->matDiffTrackBar->TabIndex = 25;
			this->matDiffTrackBar->Tag = L"";
			this->matDiffTrackBar->Value = 10;
			this->matDiffTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::MatDiffChange);
			// 
			// sheenLabel
			// 
			this->sheenLabel->AutoSize = true;
			this->sheenLabel->Location = System::Drawing::Point(666, 438);
			this->sheenLabel->Name = L"sheenLabel";
			this->sheenLabel->Size = System::Drawing::Size(36, 13);
			this->sheenLabel->TabIndex = 29;
			this->sheenLabel->Text = L"sheen";
			// 
			// matSheenTrackBar
			// 
			this->matSheenTrackBar->AutoSize = false;
			this->matSheenTrackBar->Location = System::Drawing::Point(710, 438);
			this->matSheenTrackBar->Name = L"matSheenTrackBar";
			this->matSheenTrackBar->Size = System::Drawing::Size(64, 23);
			this->matSheenTrackBar->TabIndex = 28;
			this->matSheenTrackBar->Tag = L"";
			this->matSheenTrackBar->Value = 5;
			this->matSheenTrackBar->Scroll += gcnew System::EventHandler(this, &RenderForm::MatSheenChange);
			// 
			// imageSaveButton
			// 
			this->imageSaveButton->Location = System::Drawing::Point(113, 11);
			this->imageSaveButton->Name = L"imageSaveButton";
			this->imageSaveButton->Size = System::Drawing::Size(43, 35);
			this->imageSaveButton->TabIndex = 30;
			this->imageSaveButton->Text = L"frame save";
			this->imageSaveButton->UseVisualStyleBackColor = true;
			this->imageSaveButton->Click += gcnew System::EventHandler(this, &RenderForm::ImageSave);
			// 
			// depthCheckBox
			// 
			this->depthCheckBox->AutoSize = true;
			this->depthCheckBox->Location = System::Drawing::Point(683, 49);
			this->depthCheckBox->Name = L"depthCheckBox";
			this->depthCheckBox->Size = System::Drawing::Size(73, 17);
			this->depthCheckBox->TabIndex = 31;
			this->depthCheckBox->Text = L"depth test";
			this->depthCheckBox->UseVisualStyleBackColor = true;
			this->depthCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::DepthTestSwitch);
			// 
			// modelSaveButton
			// 
			this->modelSaveButton->Location = System::Drawing::Point(162, 11);
			this->modelSaveButton->Name = L"modelSaveButton";
			this->modelSaveButton->Size = System::Drawing::Size(43, 35);
			this->modelSaveButton->TabIndex = 32;
			this->modelSaveButton->Text = L"model save";
			this->modelSaveButton->UseVisualStyleBackColor = true;
			this->modelSaveButton->Click += gcnew System::EventHandler(this, &RenderForm::ModelSave);
			// 
			// shadingComboBox
			// 
			this->shadingComboBox->FormattingEnabled = true;
			this->shadingComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"flat", L"gourand", L"fong"});
			this->shadingComboBox->Location = System::Drawing::Point(683, 108);
			this->shadingComboBox->Name = L"shadingComboBox";
			this->shadingComboBox->Size = System::Drawing::Size(72, 21);
			this->shadingComboBox->TabIndex = 33;
			this->shadingComboBox->Text = L"flat";
			this->shadingComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &RenderForm::ShadingChange);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(680, 92);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(44, 13);
			this->label2->TabIndex = 34;
			this->label2->Text = L"shading";
			// 
			// activateLightButton
			// 
			this->activateLightButton->Location = System::Drawing::Point(683, 183);
			this->activateLightButton->Name = L"activateLightButton";
			this->activateLightButton->Size = System::Drawing::Size(70, 35);
			this->activateLightButton->TabIndex = 35;
			this->activateLightButton->Text = L"activate\r\nlight";
			this->activateLightButton->UseVisualStyleBackColor = true;
			this->activateLightButton->Click += gcnew System::EventHandler(this, &RenderForm::LightSwitch);
			// 
			// renderModeButton
			// 
			this->renderModeButton->Location = System::Drawing::Point(683, 136);
			this->renderModeButton->Name = L"renderModeButton";
			this->renderModeButton->Size = System::Drawing::Size(70, 35);
			this->renderModeButton->TabIndex = 36;
			this->renderModeButton->Text = L"depth\r\nbuffer";
			this->renderModeButton->UseVisualStyleBackColor = true;
			this->renderModeButton->Click += gcnew System::EventHandler(this, &RenderForm::RenderModeSwitch);
			// 
			// moveLightButton
			// 
			this->moveLightButton->Location = System::Drawing::Point(683, 221);
			this->moveLightButton->Name = L"moveLightButton";
			this->moveLightButton->Size = System::Drawing::Size(70, 35);
			this->moveLightButton->TabIndex = 37;
			this->moveLightButton->Text = L"move\r\nlight";
			this->moveLightButton->UseVisualStyleBackColor = true;
			this->moveLightButton->Click += gcnew System::EventHandler(this, &RenderForm::LightMove);
			// 
			// texComboBox
			// 
			this->texComboBox->FormattingEnabled = true;
			this->texComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L".jpg", L"default", L"earth", L"moon", L"donut", 
				L"floor"});
			this->texComboBox->Location = System::Drawing::Point(584, 53);
			this->texComboBox->Name = L"texComboBox";
			this->texComboBox->Size = System::Drawing::Size(76, 21);
			this->texComboBox->TabIndex = 38;
			this->texComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &RenderForm::TexLoad);
			// 
			// shadowCheckBox
			// 
			this->shadowCheckBox->AutoSize = true;
			this->shadowCheckBox->Location = System::Drawing::Point(683, 68);
			this->shadowCheckBox->Name = L"shadowCheckBox";
			this->shadowCheckBox->Size = System::Drawing::Size(83, 17);
			this->shadowCheckBox->TabIndex = 39;
			this->shadowCheckBox->Text = L"shadow test";
			this->shadowCheckBox->UseVisualStyleBackColor = true;
			this->shadowCheckBox->Click += gcnew System::EventHandler(this, &RenderForm::ShadowSwitch);
			// 
			// RenderForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(785, 598);
			this->Controls->Add(this->shadowCheckBox);
			this->Controls->Add(this->texComboBox);
			this->Controls->Add(this->moveLightButton);
			this->Controls->Add(this->renderModeButton);
			this->Controls->Add(this->activateLightButton);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->shadingComboBox);
			this->Controls->Add(this->modelSaveButton);
			this->Controls->Add(this->depthCheckBox);
			this->Controls->Add(this->imageSaveButton);
			this->Controls->Add(this->sheenLabel);
			this->Controls->Add(this->matSheenTrackBar);
			this->Controls->Add(this->matDiffLabel);
			this->Controls->Add(this->matDiffTrackBar);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->matSpecLabel);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->lightDiffLabel);
			this->Controls->Add(this->matSpecTrackBar);
			this->Controls->Add(this->lightDiffTrackBar);
			this->Controls->Add(this->lightAmbTrackBar);
			this->Controls->Add(this->lightLabel);
			this->Controls->Add(this->fillCheckBox);
			this->Controls->Add(this->modelComboBox);
			this->Controls->Add(this->carouselCheckBox);
			this->Controls->Add(this->pegTopCheckBox);
			this->Controls->Add(this->transparencyСheckBox);
			this->Controls->Add(this->scaleButton);
			this->Controls->Add(this->popButton);
			this->Controls->Add(this->infoButton);
			this->Controls->Add(this->projectButton);
			this->Controls->Add(this->translateButton);
			this->Controls->Add(this->rotateButton);
			this->Controls->Add(this->pictureBox);
			this->Name = L"RenderForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"simple 3d";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightAmbTrackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightDiffTrackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matSpecTrackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matDiffTrackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matSheenTrackBar))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void InfoMessage(System::Object^  sender, System::EventArgs^  e) {
				 String^ message = "Более подробные коментарии - в заголовочных файлах проекта.\n\n" +
								   "* ComboBox в левом верхнем углу: загрузка объекта из текстового файла или процедурное построение модели;\n" +
								   "* ComboBox в правом верхнем углу: выбор текстуры для последнего добавленного объекта (доступна загрузка из файла);\n\n" + 
								   "* Button pop: удаление последнего добавленного объекта;\n" +
								   "* Button frame save: сохранение текущего кадра в data/frames;\n" +
								   "* Button model save: сохранение последнего добавленного объекта и его характеристик в текстовый файл в data/models/saved;\n\n" +
								   "* Buttons translate, scale, rotate: активация различных режимов перемещения объектов или источника освещения:\n" +
								   " - при зажатой левой кнопке мыши осуществляются сдвиги, масштабирования вдоль и вращения вокруг осей x и y;\n " +
								   " - при зажатой правой кнопке мыши - вдоль и вокруг оси z;\n" + 
								   " - колёсико мыши позволяет менять масштаб последнего добавленного объекта по всем трём осям одновременно;\n" + 
								   "* Button parallel/orthogonal projection: выбор типа проекции (параллельная или одноточечная);\n" +
								   "* CheckBoxes peg-top, carousel: режимы волчка и карусели соответственно;\n\n" +
								   "* CheckBox flood fill: заливка граней объектов (иначе отображаются каркасные модели);\n" +
								   "* CheckBox early depth test: удаление невидимых ребер и граней по алгоритму Робертса для каждой модели;\n" +
								   "* CheckBox depth test: удаление невидимых ребер и граней при помощи буфера глубины;\n" +
								   "* CheckBox shadow test: отображение теней при помощи shadow map - буфера псевдо-глубины;\n" +
								   "* ComboBox shading: выбор режима затенения объектов (плоское, по Гуро, по Фонгу).\n\n" +
								   "* Button active/deactivate light: включение/выключение источника освещения;\n" +
								   "* Button move light/objects: выбор между перемещением объектов и перемещением источника освещения;\n" +
								   "* Button frame/depth buffer: выбор между буферами кадра и глубины;\n\n" +
								   "При построении теней и перемещении источника света сцена отображается с камерой в источнике.\n" +
								   "Доступны изменения параметров источника освещения и свойств материала последнего добавленного объекта.";
				 String^ caption = "КДЗ 1 по Компьютерной графике, Касьянов И. А., 2017";
				 MessageBoxButtons buttons = MessageBoxButtons::OK;
				 System::Windows::Forms::DialogResult result;
				 // Displays the MessageBox.
				 result = MessageBox::Show( this, message, caption, buttons );
			 }

	private: System::Void TranslateEvent(System::Object^  sender, System::EventArgs^  e) {
				 this->event_data->Type = "translate";
			 }
	private: System::Void ScaleEvent(System::Object^  sender, System::EventArgs^  e) {
				 this->event_data->Type = "scale";
			 }
	private: System::Void RotateEvent(System::Object^  sender, System::EventArgs^  e) {
				 this->event_data->Type = "rotate";
			 }
	private: System::Void EarlyDepthTestSwitch(System::Object^  sender, System::EventArgs^  e) {
				 this->render_data->early_depth_test = !render_data->early_depth_test;
			 }
	private: System::Void FillSwitch(System::Object^  sender, System::EventArgs^  e) {
				 this->render_data->fill = !render_data->fill;
			 }
	private: System::Void DepthTestSwitch(System::Object^  sender, System::EventArgs^  e) {
				 this->render_data->depth_test = !render_data->depth_test;
			 }
	private: System::Void ShadowSwitch(System::Object^  sender, System::EventArgs^  e) {
				 this->render_data->shadow_test = !render_data->shadow_test;
			 }
	
	private: System::Void Render(System::Object^  sender, System::EventArgs^  e) {
				 render_tool->Render(this->models);
				 this->pictureBox->Refresh();
			 }
	private: System::Void Render(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 render_tool->Render(this->models);
				 this->pictureBox->Refresh();
			 }
	private: System::Void ModelLoad(System::Object^  sender, System::EventArgs^  e) {
				 if(this->modelComboBox->Text != "") {
					 if(this->modelComboBox->Text == ".txt") {
						 this->loadFileDialog->InitialDirectory = data_path + "\\models";
						 if(loadFileDialog->ShowDialog() == ::System::Windows::Forms::DialogResult::OK)
							 this->Add(gcnew Model(loadFileDialog->FileName));
					 }
					 else {
						 if(this->modelComboBox->Text == "cube" ||
							this->modelComboBox->Text == "tetrahedron" ||
							this->modelComboBox->Text == "octahedron" ||
							this->modelComboBox->Text == "icosahedron" ||
							this->modelComboBox->Text == "dodecahedron")
							this->Add(gcnew Model(this->modelComboBox->Text, 80));
						 else if(this->modelComboBox->Text != "thorus" &&
								 this->modelComboBox->Text != "spiral" &&
								 this->modelComboBox->Text != "spring" &&
								 this->modelComboBox->Text != "conical_spring")
								 this->Add(gcnew Model(this->modelComboBox->Text, 80, 15));
						 else
							 this->Add(gcnew Model(this->modelComboBox->Text, 80, 0.4, 15));
					 }
				 }
			 }
	private: System::Void TexLoad(System::Object^  sender, System::EventArgs^  e) {
				 if(this->texComboBox->Text != "") {
					 if(this->texComboBox->Text == ".jpg") {
						 this->loadFileDialog->InitialDirectory = data_path + "\\textures";
						 if(models->Count > 0 && loadFileDialog->ShowDialog() == ::System::Windows::Forms::DialogResult::OK)
							models[models->Count - 1]->ApplyTexture(loadFileDialog->FileName);
					 }
					 else if(this->texComboBox->Text == "default") {
						 if(models->Count > 0)
							models[models->Count - 1]->tex_data->defined = !models[models->Count - 1]->tex_data->defined;
					 }
					 else {
						 if(models->Count > 0) 
							models[models->Count - 1]->ApplyTexture(data_path + "\\textures\\" + this->texComboBox->Text + ".jpg");
					 }
				 }
			 }
	private: System::Void ProjectSwitch(System::Object^  sender, System::EventArgs^  e) {
				 if(this->projectButton->Text == L"orthogonal\r\nprojection") {
					 render_data->projection = "orthogonal";
					 this->projectButton->Text = "perspective\r\nprojection";
				 }
				 else {
					 render_data->projection = "perspective";
					 this->projectButton->Text = "orthogonal\r\nprojection";
				 }
			 }
	private: System::Void RenderModeSwitch(System::Object^  sender, System::EventArgs^  e) {
				 render_data->depth_render_mode = !render_data->depth_render_mode;

				 String^ new_text = this->renderModeButton->Text == L"depth\r\nbuffer" ?
									L"frame\r\nbuffer" : L"depth\r\nbuffer";
				 this->renderModeButton->Text = new_text;
			 }


	private: System::Void MouseDownEvent(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 event_data->MouseHold = true;
				 event_data->MouseDown = e->Location;
				 event_data->model_num = render_tool->GetModelNum(e->Location.X, e->Location.Y);
			 }
	private: System::Void MouseUpEvent(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 event_data->MouseHold = false;
			 }
	
	private: System::Void Add(Model^  model) {
				 this->models->Add(model);
			 }
	private: System::Void Remove(System::Object^  sender, System::EventArgs^  e) {
				 if(models->Count > 0)
					 this->models->RemoveAt(models->Count - 1);
			 }

	private: System::Void Transform(List<Model^>^  models, Vector^ vector, String^ type) {
				 for each(Model^ model in models)
				 	 model->Transform(vector, type);
			 }
	private: System::Void Transform(Model^  model, Vector^  vector, String^  type) {
				 if(type == "translate")
					 model->Transform(vector, type);
				 else {
					 Vector^ shift = model->Center();
					 shift *= -1; model->Transform(shift, "translate");
					 model->Transform(vector, type);
					 shift *= -1; model->Transform(shift, "translate");
					 delete shift;
				 }
			 }
	private: System::Void Transform(Vector^ vector, String^ type) {
				 light_data->light_pos *= AlgebraTool::Transform(vector, type);
				 //render_data->camera_pos *= AlgebraTool::Transform(vector, type);
			 }

	private: System::Void MouseMoveEvent(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 if(event_data->MouseHold) {
					 Point v(e->Location.X - event_data->MouseDown.X, e->Location.Y - event_data->MouseDown.Y);
					 Vector ^transform_vector;
					 bool z = e->Button == ::System::Windows::Forms::MouseButtons::Right;
					 if(event_data->Type == "translate") {
						 double translate_x = v.X,
								translate_y = v.Y,
								translate_z = -v.Y;
						 transform_vector = z ? gcnew Vector(0, 0, translate_z) :
												gcnew Vector(translate_x, translate_y, 0);
					 }
					 else if(event_data->Type == "scale") {
						 double scale_x = 1 + double(v.X)/100,
								scale_y = 1 + double(-v.Y)/100,
								scale_z = 1 + double(-v.Y)/100;
						 transform_vector = z ? gcnew Vector(1, 1, scale_z) :
												gcnew Vector(scale_x, scale_y, 1);
					 }
					 else if(event_data->Type == "rotate") {
						 double rotate_x = -v.Y,
								rotate_y = v.X,
								rotate_z = -v.X;
						 transform_vector = z ? gcnew Vector(0, 0, rotate_z) :
												gcnew Vector(rotate_x, rotate_y, 0);
						 transform_vector *= System::Math::PI / 180;
					 }

					 if(light_data->move)
						 this->Transform(transform_vector, event_data->Type);
					 else if(event_data->model_num == -1)
						 this->Transform(this->models, transform_vector, event_data->Type);
					 else
						 this->Transform(this->models[event_data->model_num], transform_vector, event_data->Type);
					 delete transform_vector;

					 event_data->MouseDown = e->Location;

					 RenderEvent(this, gcnew System::EventArgs());
				 }
			 }

	private: System::Void MouseWheelEvent(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				 double scale = 1 + double(e->Delta)/1200;
				 Vector^ scale_vector = gcnew Vector(scale, scale, scale);
				 if(event_data->scale * scale > 0.01 && models->Count > 0) {
					 if(light_data->move)
						 this->Transform(scale_vector, "scale");
					 else
						this->Transform(models[models->Count - 1], scale_vector, "scale");
					 event_data->scale *= scale;
				 }
				 delete scale_vector;
			 }

	private: System::Void PegTopSwitch(System::Object^  sender, System::EventArgs^  e) {
				 this->pegTopTimer->Enabled = !this->pegTopTimer->Enabled;
			 }
	private: System::Void PegTop(System::Object^  sender, System::EventArgs^  e) {
				 Vector^ angles = gcnew Vector(-1, 1, 1);
				 angles *= this->event_data->speed * System::Math::PI / 180;
				 for each(Model^ model in this->models)
					this->Transform(model, angles, "rotate");
				 delete angles;
			 }

	private: System::Void CarouselSwitch(System::Object^  sender, System::EventArgs^  e) {
				 this->carouselTimer->Enabled = !this->carouselTimer->Enabled;
			 }
	private: System::Void Сarousel(System::Object^  sender, System::EventArgs^  e) {
				 Vector^ angles = gcnew Vector(1, -1, -1);
				 angles *= this->event_data->speed * System::Math::PI / 180;
				 this->Transform(this->models, angles, "rotate");
				 delete angles;
			 }

	private: System::Void LightAmbChange(System::Object^  sender, System::EventArgs^  e) {
				 this->light_data->amb = double(this->lightAmbTrackBar->Value)/100;
			 }
	private: System::Void LightDiffChange(System::Object^  sender, System::EventArgs^  e) {
				 this->light_data->diff = double(this->lightDiffTrackBar->Value)/10;
			 }

	private: System::Void MatDiffChange(System::Object^  sender, System::EventArgs^  e) {
				 if(models->Count > 0) {
					 models[models->Count - 1]->mat_data->diff = double(this->matDiffTrackBar->Value)/10;
					 models[models->Count - 1]->mat_data->amb = this->models[models->Count - 1]->mat_data->diff;
				 }
			 }
	private: System::Void MatSpecChange(System::Object^  sender, System::EventArgs^  e) {
				 if(models->Count > 0)
					 models[models->Count - 1]->mat_data->spec = double(this->matSpecTrackBar->Value)/10;
			 }
	private: System::Void MatSheenChange(System::Object^  sender, System::EventArgs^  e) {
				 if(models->Count > 0)
					 models[models->Count - 1]->mat_data->sheen = System::Math::Pow(2, this->matSheenTrackBar->Value);
			 }
	private: System::Void ImageSave(System::Object^  sender, System::EventArgs^  e) {
				 System::IO::Directory::CreateDirectory(data_path + "\\frames");
				 System::String^ name = data_path + "\\frames\\frame_";
				 int i = 0;
				 while(System::IO::File::Exists(name + i.ToString() + ".jpg"))
					 i++;
				 this->pictureBox->Image->Save(name + i.ToString() + ".jpg");
				 MessageBox::Show( this, "Current frame saved in ...\\data\\frames\\frame_" + i.ToString() + ".jpg");
				 delete name;
			 }
	private: System::Void ModelSave(System::Object^  sender, System::EventArgs^  e) {
				 if(models->Count > 0) {
					 Model^ model = this->models[models->Count - 1];
					 System::IO::Directory::CreateDirectory(data_path + "\\models\\saved\\");
					 System::String^ name = data_path + "\\models\\saved\\" + model->GetType() + "_";
					 int i = 0;
					 while(System::IO::File::Exists(name + i.ToString() + ".txt"))
						 i++;
					 model->ToFile(name + i.ToString() + ".txt");
					 MessageBox::Show( this, "Last model saved in ...\\data\\models\\saved\\" + model->GetType() + "_" + i.ToString() + ".jpg");
					 delete name;
				 }
			 }
	private: System::Void ShadingChange(System::Object^  sender, System::EventArgs^  e) {
				 if(this->shadingComboBox->Text != "")
					 render_data->shading = this->shadingComboBox->Text;
			 }
	private: System::Void LightSwitch(System::Object^  sender, System::EventArgs^  e) {
				 this->light_data->active = !this->light_data->active;
				 
				 String^ new_text = this->activateLightButton->Text == L"activate\r\nlight" ?
									L"deactivate\r\nlight" : L"activate\r\nlight";
				 this->activateLightButton->Text = new_text;
			 }
	private: System::Void LightMove(System::Object^  sender, System::EventArgs^  e) {
				 this->light_data->move = !this->light_data->move;
				 
				 String^ new_text = this->moveLightButton->Text == L"move\r\nlight" ?
									L"move\r\nobjects" : L"move\r\nlight";
				 this->moveLightButton->Text = new_text;
			 }
	};
}