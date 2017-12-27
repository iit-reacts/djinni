using System.Collections.Generic;
using Djinni.TestSuite;
using NUnit.Framework;

namespace Djinni.Testing.Unit
{
    [TestFixture]
    public class EnumTest
    {
        [Test]
        public void TestEnumKey()
        {
            var m = new Dictionary<Color, string>
            {
                {Color.Red, "red"},
                {Color.Orange, "orange"},
                {Color.Yellow, "yellow"},
                {Color.Green, "green"},
                {Color.Blue, "blue"},
                {Color.Indigo, "indigo"},
                {Color.Violet, "violet"}
            };
            Assert.That(() => TestHelpers.CheckEnumMap(m), Throws.Nothing);
        }
    }
}